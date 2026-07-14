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

#include "common/core/request/sm_delivery_ack_request/sm_delivery_ack_request.h"
#include "common/core/request/sm_transfer_request/sm_transfer_request.h"

#include "common/core/response/rrc_reconfiguration_handover_response/rrc_reconfiguration_handover_response.h"
#include "common/core/response/rrc_reconfiguration_keep_response/rrc_reconfiguration_keep_response.h"

#include "common/core/response/sm_delivery_error_response/sm_delivery_error_response.h"
#include "common/core/response/sm_delivery_report_response/sm_delivery_report_response.h"
#include "common/core/response/sm_delivery_response/sm_delivery_response.h"

#include "common/core/response/ussd_balance_response/ussd_balance_response.h"
#include "common/core/response/ussd_msisdn_response/ussd_msisdn_response.h"

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
  auto ptr = response.get();
  if (auto *handoverResponse =
          dynamic_cast<common::RrcReconfigurationHandoverResponse *>(ptr)) {
    common::imsi_t newMTimsi = handoverResponse->getMTimsi();
    bool updated = ctx.setMTimsi(newMTimsi);
    if (updated) {
      addMsg("Connected. m-timsi set: " + ctx.getMTimsi());
    } else {
      if (ctx.getMTimsi() != newMTimsi) {
        addErrorMsg("Connected. New m-timsi received, but it is already "
                    "assigned");
      } else {
        addMsg("Confirmed m-timsi: " + ctx.getMTimsi());
      }
    }
  } else if (auto *keepResponse =
                 dynamic_cast<common::RrcReconfigurationKeepResponse *>(ptr)) {
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

  common::Protocol newProtocol = *parsedProtocol;
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
  if (cmd.getSpeedDialNum() != constants::EmptySpeedDialNum) {
    auto foundMsisdn = findBySpeedDialNum(cmd.getSpeedDialNum());
    if (!foundMsisdn) {
      addErrorMsg("Unknown speed dial num");
      return;
    }

    targetMsisdn = std::move(*foundMsisdn);
  } else {
    targetMsisdn = cmd.getMsisdn();
  }

  std::string smsContent = "";
  if (!cmd.getContent().empty()) {
    smsContent = cmd.getContent();
  } else {
    smsContent = menu.getSmsContent();
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
                          addMsg("SMS sent to " + targetMsisdn);
                        }
                      });
}

void App::addSentSms(const common::msisdn_t &targetMsisdn,
                     const std::string &smsContent, unsigned int smsId) {
  auto now = std::chrono::time_point_cast<std::chrono::seconds>(
      std::chrono::system_clock::now());
  Sms sms{smsId, now, {}, "", targetMsisdn, smsContent};
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
      menu.showSentSms(sms, true);
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

void App::executeUssdCodeCommand(const MenuItemUssdCode &cmd) {
  if (!exchange.hasSignal()) {
    addErrorMsg("No signal");
    return;
  }

  menu.showMessage({"Sending USSD..."});

  auto req =
      std::make_unique<common::UssdCodeRequest>(ctx.getMTimsi(), cmd.getCode());
  auto response = exchange.sendUssd(ctx.getState(), std::move(req));

  if (!response) {
    menu.showError("Error: " + response.error());
    return;
  }

  if (auto *balanceResponse =
          dynamic_cast<common::UssdBalanceResponse *>(response->get())) {
    menu.showMessage(
        {"Balance: " + common::toStr(balanceResponse->getBalance(), 2, true) +
         " rub"});
  } else if (auto *msisdnResponse =
                 dynamic_cast<common::UssdMsisdnResponse *>(response->get())) {
    menu.showMessage({"Phone number: " + msisdnResponse->getMsisdn()});
  }

  menu.showMessage({""});
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
  if (curSmsId > MaxSmsId) {
    curSmsId = 0;
  }
  return curSmsId;
}

void App::executeCommand(const std::unique_ptr<common::MenuItem> &cmd,
                         bool &exit) {
  auto ptr = cmd.get();
  if (auto *invalidCmd = dynamic_cast<common::MenuItemInvalid *>(ptr)) {
    addErrorMsg("Error! " + invalidCmd->getError());
  } else if (dynamic_cast<MenuItemExit *>(ptr)) {
    exit = true;
  } else if (auto *activeCmd = dynamic_cast<MenuItemActive *>(ptr)) {
    executeActiveCommand(*activeCmd);
  } else if (auto *moveCmd = dynamic_cast<MenuItemMove<> *>(ptr)) {
    executeMoveCommand(*moveCmd);
  } else if (auto *protocolCmd = dynamic_cast<MenuItemProtocol *>(ptr)) {
    executeProtocolCommand(*protocolCmd);
  } else if (auto *smsCmd = dynamic_cast<MenuItemSMS *>(ptr)) {
    executeSmsCommand(*smsCmd);
  } else if (dynamic_cast<MenuItemSent *>(ptr)) {
    executeSentCommand();
  } else if (dynamic_cast<MenuItemReceived *>(ptr)) {
    executeReceivedCommand();
  } else if (auto *dialogCmd = dynamic_cast<MenuItemDialog *>(ptr)) {
    executeDialogCommand(*dialogCmd);
  } else if (dynamic_cast<MenuItemUssd *>(ptr)) {
    menu.showUssdInfo(ussdInfo);
  } else if (auto *ussdCodeCmd = dynamic_cast<MenuItemUssdCode *>(ptr)) {
    executeUssdCodeCommand(*ussdCodeCmd);
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

  std::jthread backgroundReceiver{[this]() {
    exchange.receiveFromBsInBackground(
        [this](std::unique_ptr<common::Request> response,
               const std::string &error) {
          if (!error.empty()) {
            addErrorMsg("Error: " + error);
          } else {
            handleBackgroundResponse(std::move(response));
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
  addMsg(content, common::MenuMessageType::Error);
}

void App::handleBackgroundResponse(std::unique_ptr<common::Request> response) {
  auto ptr = response.get();
  if (auto *deliveryResponse =
          dynamic_cast<common::SmDeliveryResponse *>(ptr)) {
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

      auto now = std::chrono::time_point_cast<std::chrono::seconds>(
          std::chrono::system_clock::now());
      Sms sms{deliveryResponse->getSmsId(),  {}, now,
              deliveryResponse->getMsisdn(), "", deliveryResponse->getText()};
      addSms(sms);
    }

    addDeliveryAckToExchange(deliveryResponse->getMsisdn(),
                             deliveryResponse->getSmsId());
  } else if (auto *errorResponse =
                 dynamic_cast<common::SmDeliveryErrorResponse *>(ptr)) {
    setSentSmsStatus(errorResponse->getSmsId(), SmsStatus::NotDelivered);
    std::string description = errorResponse->getDescription();
    if (!description.empty()) {
      addErrorMsg("SMS not delivered: " + description);
    }
  } else if (auto *reportResponse =
                 dynamic_cast<common::SmDeliveryReportResponse *>(ptr)) {
    setSentSmsStatus(reportResponse->getSmsId(), SmsStatus::Delivered);
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
