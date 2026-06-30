#include "app.h"

#include <csignal>
#include <iostream>
#include <thread>

#include "client/app/menu/command_info/command_info.h"
#include "client/app/menu/menu_item/menu_item_exit/menu_item_exit.h"
#include "client/app/menu/menu_item/menu_item_sms/menu_item_sms.h"
#include "common/app/menu/menu_item/menu_item_empty/menu_item_empty.h"
#include "common/app/menu/menu_item/menu_item_invalid/menu_item_invalid.h"
#include "common/app/signals/signal_handler/signal_handler.h"
#include "common/core/request/rrc_reconfiguration_handover_request/rrc_reconfiguration_handover_request.h"
#include "common/core/request/rrc_reconfiguration_keep_request/rrc_reconfiguration_keep_request.h"
#include "common/core/request/sm_delivery_ack_request/sm_delivery_ack_request.h"
#include "common/core/request/sm_delivery_error_request/sm_delivery_error_request.h"
#include "common/core/request/sm_delivery_report_request/sm_delivery_report_request.h"
#include "common/core/request/sm_delivery_request/sm_delivery_request.h"
#include "common/core/request/sm_transfer_request/sm_transfer_request.h"

namespace client {
void App::sigintHandler(int signal) {
  if (signal == SIGINT) {
    std::cout << std::endl;
    exitApp();
    std::exit(signal);
  }
}

void App::addSms(const Sms &sms) {
  std::lock_guard lock(smsListMtx);
  smsList.push_back(sms);
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
  auto req = std::make_unique<common::RrcConnectionRequest>(ctx.getImei(),
                                                            ctx.getLocation());
  exchange.addRequest(ctx.getState(), std::move(req),
                      [this](std::unique_ptr<common::Request> response,
                             const std::string &error) {
                        if (!error.empty()) {
                          addErrorMsg("Error: " + error);
                        } else {
                          handleHandoverResponse(std::move(response));
                        }
                      });
}

void App::handleHandoverResponse(std::unique_ptr<common::Request> response) {
  if (auto *handoverResponse =
          dynamic_cast<common::RrcReconfigurationHandoverRequest *>(
              response.get())) {
    common::imsi_t newMTimsi = handoverResponse->getMTimsi();
    bool updated = ctx.setMTimsi(newMTimsi);
    if (updated) {
      addMsg("BS changed. m-timsi set: " + ctx.getMTimsi());
    } else {
      if (ctx.getMTimsi() != newMTimsi) {
        addErrorMsg("BS changed. New m-timsi received, but it is already "
                    "assigned");
      } else {
        addMsg("BS changed. m-timsi not updated: " + ctx.getMTimsi());
      }
    }
  } else if (auto *keepResponse =
                 dynamic_cast<common::RrcReconfigurationKeepRequest *>(
                     response.get())) {
    addMsg("BS not changed");
  }
}

void App::executeActiveCommand(const MenuItemActive &cmd) {
  bool newActive = cmd.getActive();
  bool stateChanged = newActive != ctx.isInActive();
  if (stateChanged) {
    auto error = exchange.updateConnection(newActive);
    if (error) {
      addErrorMsg("Error updating connection: " + *error);
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

void App::executeMoveCommand(const MenuItemMove<> &cmd) {
  auto coords = cmd.getCoords();

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

void App::executeProtocolCommand(const MenuItemProtocol &cmd) {
  std::string protocolStr = cmd.getProtocol();

  auto parsedProtocol = common::protocolFromStr(protocolStr);
  if (!parsedProtocol) {
    addErrorMsg("Invalid protocol");
    return;
  }

  common::Protocol newProtocol = std::move(*parsedProtocol);

  bool protocolChanged = newProtocol != ctx.getProtocol();
  if (protocolChanged) {
    ctx.setProtocol(newProtocol);
  }

  addMsg(formChangeMessage("Protocol", protocolToStr(ctx.getProtocol()),
                           protocolChanged));
}

void App::executeSmsCommand(const MenuItemSMS &cmd) {
  if (!exchange.hasSignal()) {
    addErrorMsg("No signal");
    return;
  }

  common::msisdn_t targetMsisdn = "";
  if (cmd.getSpeedDialNum() != constants::EMPTY_SPEED_DIAL_NUM) {
    auto foundMsisdn = findBySpeedDialNum(cmd.getSpeedDialNum());
    if (!foundMsisdn) {
      addErrorMsg("Unknown speed dial num");
      return;
    }

    targetMsisdn = std::move(*foundMsisdn);
  }

  std::string smsContent = "";
  if (!cmd.getContent().empty()) {
    smsContent = cmd.getContent();
  } else {
    smsContent = menu.getMessageContent();
    if (smsContent.empty()) {
      addErrorMsg("SMS content cant be empty");
    } else {
      // удаление '\n'
      smsContent.pop_back();
    }
  }

  unsigned int smsId = generateSmsId();
  auto req = std::make_unique<common::SmTransferRequest>(
      ctx.getMTimsi(), smsId, targetMsisdn, smsContent);

  exchange.addRequest(ctx.getState(), std::move(req),
                      [this, targetMsisdn, smsContent,
                       smsId](std::unique_ptr<common::Request> response,
                              const std::string &error) {
                        if (!error.empty()) {
                          addErrorMsg("Error: " + error);
                        } else {
                          addSentSms(targetMsisdn, smsContent, smsId);
                          addMsg("SMS sent to " + targetMsisdn +
                                 " (id=" + std::to_string(smsId) + ")");
                        }
                      });
}

void App::addSentSms(const common::msisdn_t &targetMsisdn,
                     const std::string &smsContent, unsigned int smsId) {
  Sms sms{smsId,
          std::chrono::time_point_cast<std::chrono::seconds>(
              std::chrono::system_clock::now()),
          {},
          "",
          targetMsisdn,
          smsContent};
  addSms(sms);
}

void App::executeDialogCommand(const MenuItemDialog &cmd) const {
  bool showed = false;
  for (const auto &sms : smsList) {
    if (sms.receiver == cmd.getMsisdn()) {
      menu.showMenuHeaderLine();
      menu.showReceivedSms(sms);
      showed = true;
    } else if (sms.sender == cmd.getMsisdn()) {
      menu.showMenuHeaderLine();
      menu.showSentSms(sms);
      showed = true;
    }
  }

  if (!showed) {
    menu.showError("No dialog");
  } else {
    menu.showMenuHeaderLine();
  }
}

void App::executeReceivedCommand() const {
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
  } else {
    menu.showMenuHeaderLine();
  }
}

void App::executeSentCommand() const {
  bool showed = false;
  for (const auto &sms : smsList) {
    if (!sms.receiver.empty()) {
      menu.showMenuHeaderLine();
      menu.showSentSms(sms);
      showed = true;
    }
  }

  if (!showed) {
    menu.showError("No sent sms");
  } else {
    menu.showMenuHeaderLine();
  }
}

void App::addDeliveryAckToExchange(const common::msisdn_t &msisdn,
                                   unsigned int smsId) {
  auto req = std::make_unique<common::SmDeliveryAckRequest>(ctx.getMTimsi(),
                                                            smsId, msisdn);
  exchange.addRequest(ctx.getState(), std::move(req),
                      [this](std::unique_ptr<common::Request> response,
                             const std::string &error) {
                        if (!error.empty()) {
                          addErrorMsg("Error: " + error);
                        }
                      });
}

void App::exitApp() {
  exchange.stop();

  std::cout << "Exiting app..." << std::endl;
}

unsigned int App::generateSmsId() {
  curSmsId++;
  if (curSmsId > MAX_SMS_ID) {
    curSmsId = 0;
  }
  return curSmsId;
}

void App::executeCommand(const std::unique_ptr<common::MenuItem> &cmd,
                         bool &exit) {
  // выполнение команды в засимости от ее типа
  if (auto *invalidCmd = dynamic_cast<common::MenuItemInvalid *>(cmd.get())) {
    addErrorMsg("Error! " + invalidCmd->getError());
  } else if (dynamic_cast<MenuItemExit *>(cmd.get())) {
    exit = true;
  } else if (auto *activeCmd = dynamic_cast<MenuItemActive *>(cmd.get())) {
    executeActiveCommand(*activeCmd);
  } else if (auto *moveCmd = dynamic_cast<MenuItemMove<> *>(cmd.get())) {
    executeMoveCommand(*moveCmd);
  } else if (auto *protocolCmd = dynamic_cast<MenuItemProtocol *>(cmd.get())) {
    executeProtocolCommand(*protocolCmd);
  } else if (auto *smsCmd = dynamic_cast<MenuItemSMS *>(cmd.get())) {
    executeSmsCommand(*smsCmd);
  } else if (auto *sentCmd = dynamic_cast<MenuItemSent *>(cmd.get())) {
    executeSentCommand();
  } else if (auto *receivedCmd = dynamic_cast<MenuItemReceived *>(cmd.get())) {
    executeReceivedCommand();
  } else if (auto *dialogCmd = dynamic_cast<MenuItemDialog *>(cmd.get())) {
    executeDialogCommand(*dialogCmd);
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
  running = true;

  std::jthread requestsSender{[this]() { exchange.sendRequests(); }};

  std::jthread smsStatusReceiver{[this]() {
    exchange.receiveSmsStatus([this](std::unique_ptr<common::Request> response,
                                     const std::string &error) {
      if (!error.empty()) {
        addErrorMsg("Error: " + error);
      } else {
        handleSmsStatusResponse(std::move(response));
      }
    });
  }};

  while (running) {
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
    executeCommand(cmd, exit);
    showMessages();

    if (exit) {
      running = false;
    } else {
      std::cout << std::endl;
    }
  }

  exitApp();
}

void App::addMsg(const std::string &content, common::MenuMessageType type) {
  std::lock_guard lock(messagesMtx);
  messages.emplace(content, type);
}

void App::addErrorMsg(const std::string &content) {
  addMsg(content, common::MenuMessageType::ERR);
}

void App::handleSmsStatusResponse(std::unique_ptr<common::Request> response) {
  if (auto *deliveryResponse =
          dynamic_cast<common::SmDeliveryRequest *>(response.get())) {
    if (deliveryResponse->getMTimsi() != ctx.getMTimsi()) {
      addErrorMsg("Unknown m-timsi in delivery response: " +
                  deliveryResponse->getMTimsi());
      return;
    }

    bool duplicate = false;
    for (const auto &sms : smsList) {
      if (sms.id == deliveryResponse->getSmsId() &&
          sms.sender == deliveryResponse->getMsisdn()) {
        duplicate = true;
        break;
      }
    }

    if (!duplicate) {
      addMsg("SMS received from " + deliveryResponse->getMsisdn());

      Sms sms{deliveryResponse->getSmsId(),
              {},
              std::chrono::time_point_cast<std::chrono::seconds>(
                  std::chrono::system_clock::now()),
              deliveryResponse->getMsisdn(),
              "",
              deliveryResponse->getText()};
      addSms(sms);
    }

    addDeliveryAckToExchange(deliveryResponse->getMsisdn(),
                             deliveryResponse->getSmsId());
  } else if (auto *errorResponse =
                 dynamic_cast<common::SmDeliveryErrorRequest *>(
                     response.get())) {
    setSentSmsStatus(errorResponse->getSmsId(), SmsStatus::NOT_DELIVERED);
  } else if (auto *reportResponse =
                 dynamic_cast<common::SmDeliveryReportRequest *>(
                     response.get())) {
    setSentSmsStatus(reportResponse->getSmsId(), SmsStatus::DELIVERED);
  }
}

void App::showMessages() {
  std::lock_guard lock(messagesMtx);
  menu.showMessages(messages);
}

void App::setSentSmsStatus(unsigned int smsId, SmsStatus status) {
  std::lock_guard lock(smsListMtx);

  for (int i = 0; i < smsList.size(); ++i) {
    if (smsList[i].id == smsId && smsList[i].sender.empty()) {
      smsList[i].status = status;
      return;
    }
  }
}
} // namespace client
