#include "app.h"

#include <csignal>
#include <iostream>
#include <spdlog/fmt/fmt.h>
#include <thread>

#include "client/app/menu/command_info/command_info.h"
#include "client/app/menu/menu_item/menu_item_exit/menu_item_exit.h"
#include "client/app/menu/menu_item/menu_item_sms/menu_item_sms.h"
#include "common/app/menu/menu_item/menu_item_empty/menu_item_empty.h"
#include "common/app/menu/menu_item/menu_item_invalid/menu_item_invalid.h"
#include "common/app/signals/signal_handler/signal_handler.h"
#include "common/core/request/rrc_reconfiguration_handover_request/rrc_reconfiguration_handover_request.h"
#include "common/core/request/rrc_reconfiguration_keep_request/rrc_reconfiguration_keep_request.h"

namespace client {
void App::sigintHandler(int signal) {
  if (signal == SIGINT) {
    exchange.stop();

    if (common::Logger::isInitialized()) {
      common::Logger::instance().getInner()->flush();
    }

    std::cout << std::endl << "Exiting app..." << std::endl;
    std::exit(signal);
  }
}

std::string App::formChangeMessage(const std::string &paramName,
                                   const std::string &valueStr,
                                   bool changed) const {
  std::string content = paramName;
  content += changed ? " changed to " : " already set to ";
  content += valueStr;
  return content;
}

void App::handleLocationUpdate() {
  exchange.addRequest(
      ctx.getState(), common::RequestType::Rrc_Connection,
      [this](std::unique_ptr<common::Request> response,
             const std::string &error) {
        if (!error.empty()) {
          addErrorMsg("Error: " + error);
          return;
        }

        if (auto *handoverResponse =
                dynamic_cast<common::RrcReconfigurationHandoverRequest *>(
                    response.get())) {
          common::imsi_t newMTimsi = handoverResponse->getMTimsi();
          bool updated = ctx.setMTimsi(newMTimsi);
          if (!updated) {
            if (ctx.getMTimsi() != newMTimsi) {
              addErrorMsg(
                  "Handover. New m-timsi received, but it is already assigned");
            } else {
              addMsg("Handover. Confirmed m-timsi: " + ctx.getMTimsi());
            }

            return;
          }

          addMsg("Handover. m-timsi set: " + ctx.getMTimsi());
        }
      });
}

void App::handleActiveCommand(const MenuItemActive &cmd) {
  logCommandProcess(cmd.getName(), fmt::format("active={}", cmd.getActive()));

  bool newActive = cmd.getActive();
  bool stateChanged = newActive != ctx.isInActive();
  if (stateChanged) {
    auto updateError = exchange.updateConnection(newActive);
    if (updateError) {
      addErrorMsg("Error updating connection: " + *updateError);
      return;
    }

    ctx.setInActive(newActive);
    if (ctx.isInActive()) {
      handleLocationUpdate();
    }
  }

  addMsg(formChangeMessage("State", ueActiveToStr(ctx.isInActive()),
                           stateChanged));
}

void App::handleMoveCommand(const MenuItemMove<> &cmd) {
  auto coords = cmd.getCoords();
  logCommandProcess(
      cmd.getName(),
      fmt::format("coords={}", common::toStr(coords.begin(), coords.end())));

  bool locationChanged = !ctx.getLocation().coordsEqual(coords);
  try {
    if (locationChanged) {
      ctx.updateLocation(coords);
      if (ctx.isInActive()) {
        if (!exchange.hasSignal()) {
          addErrorMsg("No signal. Try to reconnect (active 0, active 1)");
        } else {
          handleLocationUpdate();
        }
      }
    }

    addMsg(formChangeMessage("Location", ctx.getLocation().toStr(),
                             locationChanged));
  } catch (const std::invalid_argument &e) {
    addErrorMsg("Location coords count is invalid");
  }
}

void App::handleProtocolCommand(const MenuItemProtocol &cmd) {
  std::string protocolStr = cmd.getProtocol();
  logCommandProcess(cmd.getName(), fmt::format("protocol={}", protocolStr));

  auto parsedProtocol = common::protocolFromStr(protocolStr);
  if (parsedProtocol) {
    common::Protocol newProtocol = std::move(*parsedProtocol);

    bool protocolChanged = newProtocol != ctx.getProtocol();
    if (protocolChanged) {
      ctx.setProtocol(newProtocol);
    }

    addMsg(formChangeMessage("Protocol", protocolToStr(ctx.getProtocol()),
                             protocolChanged));
    return;
  }

  addErrorMsg("Invalid protocol");
}

void App::handleSmsCommand(const MenuItemSMS &cmd) {
  common::msisdn_t targetMsisdn = "";
  if (cmd.getSpeedDialNum() != constants::EMPTY_SPEED_DIAL_NUM) {
    auto foundMsisdn = findBySpeedDialNum(cmd.getSpeedDialNum());
    if (!foundMsisdn) {
      addErrorMsg("Unknown speed dial num");
      return;
    } else {
      targetMsisdn = std::move(*foundMsisdn);
    }
  }

  std::string smsContent = "";
  if (!cmd.getContent().empty()) {
    smsContent = cmd.getContent();
  } else {
    smsContent = menu.getMessageContent();
    if (smsContent.empty()) {
      addErrorMsg("SMS content cant be empty");
      return;
    } else {
      // удаление '\n'
      smsContent.pop_back();
    }
  }

  addMsg("SMS: " + targetMsisdn + ", " + smsContent);
}

void App::handleDialogCommand(const MenuItemDialog &cmd) const {
  bool showed = false;
  for (const auto &sms : smsList) {
    menu.showMenuHeaderLine();
    if (sms.receiver == cmd.getMsisdn()) {
      menu.showSentSms(sms);
      showed = true;
    } else if (sms.sender == cmd.getMsisdn()) {
      menu.showReceivedSms(sms);
      showed = true;
    }
  }

  if (!showed) {
    menu.showError("No dialog found");
  }
}

void App::handleReceivedCommand() const {
  bool showed = false;
  for (const auto &sms : smsList) {
    if (sms.receiver.empty()) {
      menu.showMenuHeaderLine();
      menu.showReceivedSms(sms);
      showed = true;
    }
  }

  if (!showed) {
    menu.showError("No received sms");
  }
}

void App::handleSentCommand() const {
  bool showed = false;
  for (const auto &sms : smsList) {
    menu.showMenuHeaderLine();
    if (!sms.receiver.empty()) {
      menu.showSentSms(sms);
      showed = true;
    }
  }

  if (!showed) {
    menu.showError("No sent sms");
  }
}

void App::handleCommand(const std::unique_ptr<common::MenuItem> &cmd,
                        bool &exit) {
  std::string cmdNameUpper = common::uppercased(cmd->getName());
  bool isCorrectCommand = true;

  // выполнение команды в засимости от ее типа
  if (auto *invalidCmd = dynamic_cast<common::MenuItemInvalid *>(cmd.get())) {
    SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                       "Received invalid command: {}", invalidCmd->getError());
    addErrorMsg("Error! " + invalidCmd->getError());
    isCorrectCommand = false;
  } else if (dynamic_cast<MenuItemExit *>(cmd.get())) {
    logCommandProcess(cmdNameUpper);
    addMsg("Exiting app...");
    exit = true;
  } else if (auto *activeCmd = dynamic_cast<MenuItemActive *>(cmd.get())) {
    handleActiveCommand(*activeCmd);
  } else if (auto *moveCmd = dynamic_cast<MenuItemMove<> *>(cmd.get())) {
    handleMoveCommand(*moveCmd);
  } else if (auto *protocolCmd = dynamic_cast<MenuItemProtocol *>(cmd.get())) {
    handleProtocolCommand(*protocolCmd);
  } else if (auto *smsCmd = dynamic_cast<MenuItemSMS *>(cmd.get())) {
    handleSmsCommand(*smsCmd);
  } else if (auto *sentCmd = dynamic_cast<MenuItemSent *>(cmd.get())) {
    handleSentCommand();
  } else if (auto *receivedCmd = dynamic_cast<MenuItemReceived *>(cmd.get())) {
    handleReceivedCommand();
  } else if (auto *dialogCmd = dynamic_cast<MenuItemDialog *>(cmd.get())) {
    handleDialogCommand(*dialogCmd);
  } else if (auto *emptyCmd =
                 dynamic_cast<common::MenuItemEmpty *>(cmd.get())) {
    return;
  }

  if (isCorrectCommand) {
    SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                       "Finished command {}", cmdNameUpper);
  }
}

std::optional<common::msisdn_t> App::findBySpeedDialNum(char num) {
  auto it = addressBook.find(num);
  if (it == addressBook.end()) {
    return std::nullopt;
  }
  return it->second;
}

App::App(const UeContext &ctx_,
         const std::map<char, common::msisdn_t> &addressBook_)
    : ctx(ctx_), addressBook(addressBook_), exchange(ctx_.getServerAddr()) {
  common::SignalHandler::setHandler(
      SIGINT, [this](int signal) { sigintHandler(signal); });
}

void App::run() {
  messages = {};
  isRunning = true;

  std::jthread requestsHandler{[this]() { exchange.handleRequests(); }};

  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(), "App started");
  while (isRunning) {
    menu.showMenuHeaderLine();
    menu.showStatus(ctx.isInActive(), ctx.getImsi(), ctx.getProtocol());
    menu.showMenuHeaderLine();
    menu.showSignalInfo(ctx.getLocation(), exchange.getSignalLevel());
    menu.showMenuHeaderLine();
    menu.showAddressBook(addressBook);
    menu.showMenuHeaderLine();
    menu.showCommandsInfo(getCommandsInfo());

    std::string extraMsgContent = "";
    auto cmd = menu.getCommand(extraMsgContent);

    if (!extraMsgContent.empty()) {
      menu.showMessage({extraMsgContent});
    }

    bool exit = false;
    handleCommand(cmd, exit);
    menu.showMessages(messages);

    if (exit) {
      isRunning = false;
    } else {
      std::cout << std::endl;
    }
  }

  exchange.stop();

  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(), "App exited");
}

void App::logCommandProcess(std::string_view commandName,
                            std::string_view argsStr) const {
  std::string nameUpper = common::uppercased(commandName);
  if (!argsStr.empty()) {
    SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                       "Processing command {} with args: {}", nameUpper,
                       argsStr);
  } else {
    SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                       "Processing command {}", nameUpper);
  }
}

void App::addMsg(const std::string &content, common::MenuMessageType type) {
  messages.emplace(content, type);
}

void App::addErrorMsg(const std::string &content) {
  addMsg(content, common::MenuMessageType::ERR);
}
} // namespace client
