#include "app.h"

#include <csignal>
#include <iostream>
#include <spdlog/fmt/fmt.h>
#include <stdexcept>
#include <thread>

#include "client/app/menu/menu_item/menu_item_dialog/menu_item_dialog.h"
#include "client/app/menu/menu_item/menu_item_sms/menu_item_sms.h"
#include "client/network/socket/socket.h"
#include "common/app/menu/menu_item/menu_item_exit/menu_item_exit.h"
#include "common/app/menu/menu_item/menu_item_invalid/menu_item_invalid.h"
#include "common/app/signals/signal_handler/signal_handler.h"
#include "common/utils/str/str.h"

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

common::MenuMessage App::formChangeMessage(const std::string &paramName,
                                           const std::string &valueStr,
                                           bool changed) const {
  std::string content = paramName;
  content += changed ? " changed to " : " already set to ";
  content += valueStr;
  return common::MenuMessage{content};
}

void App::handleLocationUpdate() {
  auto req = std::make_unique<common::RrcConnectionRequest>(ctx.getImsi(),
                                                            ctx.getLocation());

  exchange.addRequest(
      ctx.getProtocol(), common::RequestType::Rrc_Connection, std::move(req),
      [this](std::unique_ptr<common::Request> reqponse,
             const std::string &error) {
        if (!error.empty()) {
          messages.push({"Error sending location to server: " + error,
                         common::MenuMessageType::ERR});
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
      messages.push({"Error updating connection: " + *updateError,
                     common::MenuMessageType::ERR});
      return;
    }

    ctx.setInActive(newActive);
    if (ctx.isInActive()) {
      handleLocationUpdate();
    }
  }

  messages.push(formChangeMessage("State", ueActiveToStr(ctx.isInActive()),
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
        handleLocationUpdate();
      }
    }

    messages.push(formChangeMessage("Location", ctx.getLocation().toStr(),
                                    locationChanged));
  } catch (const std::invalid_argument &e) {
    messages.push({"Location coords count is invalid"});
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

    messages.push({formChangeMessage(
        "Protocol", protocolToStr(ctx.getProtocol()), protocolChanged)});
    return;
  }

  messages.push({"Invalid protocol"});
}

void App::handleSmsCommand(const MenuItemSMS &cmd) {
  common::msisdn_t targetMsisdn = "";
  if (cmd.getSpeedDialNum() != constants::EMPTY_SPEED_DIAL_NUM) {
    auto foundMsisdn = findBySpeedDialNum(cmd.getSpeedDialNum());
    if (!foundMsisdn) {
      messages.push({"Unknown speed dial num"});
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
      messages.push(
          {"SMS content cant be empty", common::MenuMessageType::ERR});
    } else {
      // удаление '\n'
      smsContent.pop_back();
    }
  }

  messages.push({"SMS: " + targetMsisdn + ", " + smsContent});
}

void App::handleCommand(const std::unique_ptr<common::MenuItem> &cmd,
                        bool &exit) {
  std::string cmdNameUpper = common::uppercased(cmd->getName());
  bool isCorrectCommand = true;

  // выполнение команды в засимости от ее типа
  if (auto *invalidCmd = dynamic_cast<common::MenuItemInvalid *>(cmd.get())) {
    SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                       "Received invalid command: {}", invalidCmd->getError());
    messages.push(
        {"Error! " + invalidCmd->getError(), common::MenuMessageType::ERR});
    isCorrectCommand = false;
  } else if (dynamic_cast<common::MenuItemExit *>(cmd.get())) {
    logCommandProcess(cmdNameUpper);
    messages.push({"Exiting app..."});
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
    messages.push({"SENT"});
  } else if (auto *receivedCmd = dynamic_cast<MenuItemReceived *>(cmd.get())) {
    messages.push({"RECEIVED"});
  } else if (auto *dialogCmd = dynamic_cast<MenuItemDialog *>(cmd.get())) {
    messages.push({"DIALOG"});
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
    : ctx(ctx_), addressBook(addressBook_), exchange(ctx.getServerAddr()) {
  common::SignalHandler::setHandler(
      SIGINT, [this](int signal) { sigintHandler(signal); });
}

void App::run() {
  messages = {};
  isRunning = true;

  std::thread requestsHandler{[this]() { exchange.handleRequests(); }};

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
  requestsHandler.join();

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
} // namespace client
