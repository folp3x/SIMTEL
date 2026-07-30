#include "app.h"

#include <csignal>
#include <iostream>
#include <thread>

#include "app/menu/command_info/command_info.h"
#include "app/menu/menu_item/menu_item_exit/menu_item_exit.h"
#include "app/menu/menu_item/menu_item_sms/menu_item_sms.h"

#include "common/app/menu/menu_item/menu_item_invalid/menu_item_invalid.h"
#include "common/app/signal_handler/signal_handler.h"

#include "common/core/request/sm_delivery_ack_request/sm_delivery_ack_request.h"
#include "common/core/request/sm_transfer_request/sm_transfer_request.h"

#include "common/core/response/rrc_reconfiguration_keep_response/rrc_reconfiguration_keep_response.h"

#include "common/core/response/sm_delivery_error_response/sm_delivery_error_response.h"
#include "common/core/response/sm_delivery_report_response/sm_delivery_report_response.h"

#include "common/core/response/ussd_balance_response/ussd_balance_response.h"
#include "common/core/response/ussd_msisdn_response/ussd_msisdn_response.h"

#include "common/utils/time/time.h"

namespace client {
bool App::isSentSms(const Sms &sms) { return sms.sender.empty(); }

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

bool App::duplicateSmsReceived(unsigned int smsId,
                               const common::msisdn_t &msisdn_s) const {
  for (const auto &sms : smsList) {
    if (sms.id == smsId && sms.sender == msisdn_s) {
      return true;
    }
  }
  return false;
}

std::string App::formChangeMessage(const std::string &paramName,
                                   const std::string &valueStr,
                                   bool changed) const {
  std::string content = paramName + " ";
  content += changed ? _("changed to") : _("already set to");
  content += " " + valueStr;
  return content;
}

void App::handleLocationUpdate() {
  auto req = std::make_unique<common::RrcConnectionRequest>(
      ctx.getImei(), ctx.getLocation().getCoords());
  exchange.addRequest(ctx.getState(), std::move(req),
                      [this](std::unique_ptr<common::Request> response,
                             const std::string &error) {
                        if (!error.empty()) {
                          addErrorMsg(ErrorMsgPrefix + error);
                        } else {
                          handleLocationUpdateResponse(std::move(response));
                        }
                      });
}

void App::handleLocationUpdateResponse(
    std::unique_ptr<common::Request> response) {
  auto ptr = response.get();
  if (auto *handoverResponse =
          dynamic_cast<common::RrcReconfigurationHandoverResponse *>(ptr)) {
    handleHandoverResponse(*handoverResponse);
  } else if (auto *keepResponse =
                 dynamic_cast<common::RrcReconfigurationKeepResponse *>(ptr)) {
    addMsg(_("BS not changed"));
  }
}

void App::handleHandoverResponse(
    const common::RrcReconfigurationHandoverResponse &response) {
  common::imsi_t newMTimsi = response.getMTimsi();
  bool updated = ctx.setMTimsi(newMTimsi);

  if (updated) {
    std::string msgContent = _("Connected. m-timsi set");
    msgContent += ": " + ctx.getMTimsi();
    addMsg(msgContent);
  } else {
    if (ctx.getMTimsi() != newMTimsi) {
      addErrorMsg(_("Connected. New m-timsi received, but it is already "
                    "assigned"));
    } else {
      std::string msgContent = _("Confirmed m-timsi");
      msgContent += ": " + ctx.getMTimsi();
      addMsg(msgContent);
    }
  }
}

void App::executeActiveCommand(const MenuItemActive &cmd) {
  bool newActive = cmd.getActive();
  bool stateChanged = newActive != ctx.isInActive();
  if (stateChanged) {
    auto error = exchange.updateConnection(newActive);
    if (error) {
      std::string msgContent = _("Connection error");
      msgContent += ": " + *error;
      addErrorMsg(msgContent);
      return;
    }

    ctx.setInActive(newActive);
    if (ctx.isInActive()) {
      handleLocationUpdate();
    }
  }

  addMsg(formChangeMessage(_("State"), ueActiveToStr(ctx.isInActive()),
                           stateChanged));
}

void App::executeMoveCommand(const MenuItemMove<> &cmd) {
  auto coords = cmd.getCoords();

  bool locationChanged = !ctx.getLocation().coordsEqual(coords);
  if (locationChanged) {
    ctx.updateLocation(coords);
    if (ctx.isInActive()) {
      if (!exchange.hasSignal()) {
        addErrorMsg(_("No signal. Try to reconnect (active 0, active 1)"));
      } else {
        handleLocationUpdate();
      }
    }
  }

  addMsg(formChangeMessage(_("Location"), ctx.getLocation().toStr(),
                           locationChanged));
}

void App::executeProtocolCommand(const MenuItemProtocol &cmd) {
  std::string protocolStr = cmd.getProtocol();

  auto parsedProtocol = common::protocolFromStr(protocolStr);
  if (!parsedProtocol) {
    addErrorMsg(_("Invalid protocol"));
    return;
  }

  common::Protocol newProtocol = *parsedProtocol;
  bool protocolChanged = newProtocol != ctx.getProtocol();
  if (protocolChanged) {
    ctx.setProtocol(newProtocol);
  }

  addMsg(formChangeMessage(_("Protocol"), protocolToStr(ctx.getProtocol()),
                           protocolChanged));
}

void App::executeSmsCommand(const MenuItemSMS &cmd) {
  if (!exchange.hasSignal()) {
    addErrorMsg(NoSignalMsg);
    return;
  }

  common::msisdn_t targetMsisdn;
  if (cmd.getSpeedDialNum() != constants::EmptySpeedDialNum) {
    auto foundMsisdn = findBySpeedDialNum(cmd.getSpeedDialNum());
    if (!foundMsisdn) {
      addErrorMsg(_("Unknown speed dial num"));
      return;
    }

    targetMsisdn = std::move(*foundMsisdn);
  } else {
    targetMsisdn = cmd.getMsisdn();
  }

  std::string smsContent;
  if (!cmd.getContent().empty()) {
    smsContent = cmd.getContent();
  } else {
    smsContent = menu.getSmsContent();
    if (smsContent.empty()) {
      addErrorMsg(_("SMS content cant be empty"));
      return;
    }
  }

  sendSms(targetMsisdn, smsContent);
}

void App::sendSms(const common::msisdn_t &targetMsisdn,
                  const std::string &smsContent) {
  unsigned int smsId = generateSmsId();
  auto req = std::make_unique<common::SmTransferRequest>(
      ctx.getMTimsi(), smsId, targetMsisdn, smsContent);

  exchange.addRequest(ctx.getState(), std::move(req),
                      [this, targetMsisdn, smsContent,
                       smsId](std::unique_ptr<common::Request> response,
                              const std::string &error) {
                        if (!error.empty()) {
                          addErrorMsg(ErrorMsgPrefix + error);
                        } else {
                          addSentSms(targetMsisdn, smsContent, smsId);

                          std::string msgContent = _("SMS sent to");
                          msgContent += " " + targetMsisdn;
                          addMsg(msgContent);
                        }
                      });
}

void App::addSentSms(const common::msisdn_t &targetMsisdn,
                     const std::string &smsContent, unsigned int smsId) {
  auto now = common::utils::getNowSeconds();
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
    menu.showError(_("No dialog"));
  } else {
    menu.showMenuHeaderLine();
  }
}

void App::executeReceivedCommand() const {
  bool showed = false;
  for (const auto &sms : smsList) {
    if (!isSentSms(sms)) {
      menu.showMenuHeaderLine();
      menu.showReceivedSms(sms);
      showed = true;
    }
  }

  if (!showed) {
    menu.showError(_("No received sms"));
  } else {
    menu.showMenuHeaderLine();
  }
}

void App::executeSentCommand() const {
  bool showed = false;
  for (const auto &sms : smsList) {
    if (isSentSms(sms)) {
      menu.showMenuHeaderLine();
      menu.showSentSms(sms);
      showed = true;
    }
  }

  if (!showed) {
    menu.showError(_("No sent sms"));
  } else {
    menu.showMenuHeaderLine();
  }
}

void App::executeUssdCodeCommand(const MenuItemUssdCode &cmd) {
  if (!exchange.hasSignal()) {
    addErrorMsg(NoSignalMsg);
    return;
  }

  menu.showMessage({_("Sending USSD...")});

  auto req =
      std::make_unique<common::UssdCodeRequest>(ctx.getMTimsi(), cmd.getCode());
  auto response = exchange.sendUssd(ctx.getState(), std::move(req));

  if (!response) {
    menu.showError(ErrorMsgPrefix + response.error());
  } else {
    handleUssdResponse(std::move(*response));
  }
}

void App::handleUssdResponse(std::unique_ptr<common::Request> response) {
  auto ptr = response.get();
  if (auto *balanceResponse =
          dynamic_cast<common::UssdBalanceResponse *>(ptr)) {
    std::string msgContent = _("Balance");
    std::string balanceStr =
        common::utils::toStr(balanceResponse->getBalance(),
                             common::constants::PriceNumPrecision, true);
    msgContent += ": " + balanceStr + " " + _("rub");
    menu.showMessage({msgContent});
  } else if (auto *msisdnResponse =
                 dynamic_cast<common::UssdMsisdnResponse *>(ptr)) {
    std::string msgContent = _("Phone number");
    msgContent += ": " + msisdnResponse->getMsisdn();
    menu.showMessage({msgContent});
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
                          addErrorMsg(ErrorMsgPrefix + error);
                        }
                      });
}

void App::exitApp() {
  exchange.stop();

  std::cout << _("Exiting app...") << std::endl;
}

unsigned int App::generateSmsId() {
  curSmsId++;
  if (curSmsId > MaxSmsId) {
    curSmsId = 1;
  }
  return curSmsId;
}

void App::executeCommand(const std::unique_ptr<common::MenuItem> &cmd,
                         bool &exit) {
  auto ptr = cmd.get();
  if (auto *invalidCmd = dynamic_cast<common::MenuItemInvalid *>(ptr)) {
    addErrorMsg(ErrorMsgPrefix + invalidCmd->getError());
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
  auto records = addressBook.getRecords();
  auto it = records.find(num);
  if (it == records.end()) {
    return std::nullopt;
  }
  return it->second;
}

App::App(const UeContext &ctx_, const AddressBook &addressBook_)
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
            addErrorMsg(ErrorMsgPrefix + error);
          } else {
            handleBackgroundResponse(std::move(response));
          }
        });
  }};

  while (running) {
    showMenu();

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
    handleSmDeliveryResponse(*deliveryResponse);
  } else if (auto *errorResponse =
                 dynamic_cast<common::SmDeliveryErrorResponse *>(ptr)) {
    setSentSmsStatus(errorResponse->getSmsId(), SmsStatus::NotDelivered);
    std::string description = errorResponse->getDescription();
    if (!description.empty()) {
      std::string msgContent = _("SMS not delivered");
      msgContent += ": " + description;
      addErrorMsg(msgContent + description);
    }
  } else if (auto *reportResponse =
                 dynamic_cast<common::SmDeliveryReportResponse *>(ptr)) {
    setSentSmsStatus(reportResponse->getSmsId(), SmsStatus::Delivered);
  }
}

void App::handleSmDeliveryResponse(const common::SmDeliveryResponse &response) {
  if (response.getMTimsi() != ctx.getMTimsi()) {
    std::string msgContent = _("Unknown m-timsi in delivery response");
    msgContent += ": " + response.getMTimsi();
    addErrorMsg(msgContent);
    return;
  }

  unsigned int smsId = response.getSmsId();
  common::msisdn_t msisdn_s = response.getMsisdn();
  bool duplicate = duplicateSmsReceived(smsId, msisdn_s);

  if (!duplicate) {
    std::string msgContent = _("SMS received from");
    msgContent += " " + msisdn_s;
    addMsg(msgContent);

    auto now = common::utils::getNowSeconds();
    Sms sms{smsId, {}, now, msisdn_s, "", response.getText()};
    addSms(sms);
  }

  addDeliveryAckToExchange(msisdn_s, smsId);
}

void App::showMessages() {
  std::lock_guard lock(messagesMtx);
  menu.showMessages(messages);
}

void App::setSentSmsStatus(unsigned int smsId, SmsStatus status) {
  std::lock_guard lock(smsListMtx);

  for (size_t i = 0; i < smsList.size(); ++i) {
    if (smsList[i].id == smsId && isSentSms(smsList[i])) {
      smsList[i].status = status;
      return;
    }
  }
}

void App::showMenu() const {
  menu.showMenuHeaderLine();
  menu.showStatus(ctx.isInActive(), ctx.getImsi(), ctx.getProtocol());
  menu.showMenuHeaderLine();
  menu.showSignalInfo(ctx.getLocation(), exchange.getSignalLevel());
  menu.showMenuHeaderLine();
  menu.showAddressBook(addressBook);
  menu.showMenuHeaderLine();
  menu.showCommandsInfo(getCommandsInfo());
}
} // namespace client
