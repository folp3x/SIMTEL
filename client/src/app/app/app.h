#pragma once

#include "app/config/config/config.h"

#include "app/menu/menu/menu.h"

#include "app/menu/menu_item/menu_item_active/menu_item_active.h"
#include "app/menu/menu_item/menu_item_dialog/menu_item_dialog.h"
#include "app/menu/menu_item/menu_item_move/menu_item_move.h"
#include "app/menu/menu_item/menu_item_protocol/menu_item_protocol.h"

#include "app/menu/menu_item/menu_item_sms/menu_item_sms.h"
#include "app/menu/menu_item/menu_item_ussd_code/menu_item_ussd_code.h"

#include "app/address_book/address_book.h"

#include "core/sms/sms/sms.h"

#include "core/ue/ue_active/ue_active.h"
#include "core/ue/ue_context/ue_context.h"
#include "core/ue/ue_exchange/ue_exchange.h"

#include "common/core/request/rrc_connection_request/rrc_connection_request.h"
#include "common/core/response/rrc_reconfiguration_handover_response/rrc_reconfiguration_handover_response.h"
#include "common/core/response/sm_delivery_response/sm_delivery_response.h"

namespace client {
class App {
private:
  static constexpr unsigned int MaxSmsId = 9999;

  inline static const std::string ErrorMsgPrefixText = _("Error");
  inline static const std::string ErrorMsgPrefix = ErrorMsgPrefixText + ": ";
  inline static const std::string NoSignalMsg = _("No signal");

  std::vector<UssdInfo> ussdInfo = {
      {common::UssdCode::GetBalance, "get balance"},
      {common::UssdCode::GetPhoneNumber, "get phone number"}};

  unsigned int curSmsId = 1;

  UeContext ctx;
  UeExchange exchange;

  Menu menu;

  bool running = false;

  std::mutex messagesMtx;
  std::queue<common::MenuMessage> messages{};

  AddressBook addressBook{};

  std::mutex smsListMtx;
  std::vector<Sms> smsList{};

  static bool isSentSms(const Sms &sms);

  void executeCommand(const std::unique_ptr<common::MenuItem> &cmd, bool &exit);

  void sigintHandler(int signal);

  void addSms(const Sms &sms);

  bool duplicateSmsReceived(unsigned int smsId,
                            const common::msisdn_t &msisdn_s) const;

  std::string formChangeMessage(const std::string &paramName,
                                const std::string &valueStr,
                                bool changed = true) const;

  void handleLocationUpdate();
  void handleLocationUpdateResponse(std::unique_ptr<common::Request> response);
  void handleHandoverResponse(
      const common::RrcReconfigurationHandoverResponse &response);

  void executeActiveCommand(const MenuItemActive &cmd);
  void executeMoveCommand(const MenuItemMove<> &cmd);
  void executeProtocolCommand(const MenuItemProtocol &cmd);

  void executeSmsCommand(const MenuItemSMS &cmd);
  void sendSms(const common::msisdn_t &targetMsisdn,
               const std::string &smsContent);

  void addSentSms(const common::msisdn_t &targetMsisdn,
                  const std::string &smsContent, unsigned int smsId);

  void executeDialogCommand(const MenuItemDialog &cmd) const;
  void executeReceivedCommand() const;
  void executeSentCommand() const;

  void executeUssdCodeCommand(const MenuItemUssdCode &cmd);
  void handleUssdResponse(std::unique_ptr<common::Request> response);

  void addDeliveryAckToExchange(const common::msisdn_t &msisdn,
                                unsigned int smsId);

  void exitApp();

  unsigned int generateSmsId();

  std::optional<common::msisdn_t> findBySpeedDialNum(char num);

  void addMsg(const std::string &content,
              common::MenuMessageType type = common::MenuMessageType::Info);

  void addErrorMsg(const std::string &content);

  void handleBackgroundResponse(std::unique_ptr<common::Request> response);

  void handleSmDeliveryResponse(const common::SmDeliveryResponse &response);

  void showMessages();

  void setSentSmsStatus(unsigned int smsId, SmsStatus status);

  void showMenu() const;

public:
  App(const UeContext &ctx_, const AddressBook &addressBook_);

  void run();
};
} // namespace client
