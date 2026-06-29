#pragma once

#include "client/app/config/config/config.h"
#include "client/app/menu/menu/menu.h"
#include "client/app/menu/menu_item/menu_item_active/menu_item_active.h"
#include "client/app/menu/menu_item/menu_item_dialog/menu_item_dialog.h"
#include "client/app/menu/menu_item/menu_item_move/menu_item_move.h"
#include "client/app/menu/menu_item/menu_item_protocol/menu_item_protocol.h"
#include "client/app/menu/menu_item/menu_item_sms/menu_item_sms.h"
#include "client/core/ue/ue_active/ue_active.h"
#include "client/core/ue/ue_context/ue_context.h"
#include "client/core/ue/ue_exchange/ue_exchange.h"
#include "common/core/request/rrc_connection_request/rrc_connection_request.h"
#include "common/core/sms/sms.h"

namespace client {
class App {
private:
  static constexpr unsigned int HANDOVER_SIGNAL_THRESHOLD = 40;

  static constexpr unsigned int MAX_SMS_ID = 9999;

  unsigned int curSmsId = 0;

  UeContext ctx;
  UeExchange exchange;

  Menu menu;

  bool isRunning = false;

  std::mutex messagesMtx;
  std::priority_queue<common::MenuMessage> messages{};

  std::map<char, common::msisdn_t> addressBook{};

  std::mutex smsListMtx;
  std::vector<common::Sms> smsList{};

  virtual void handleCommand(const std::unique_ptr<common::MenuItem> &cmd,
                             bool &exit);

  void sigintHandler(int signal);

  void addSms(const common::Sms &sms);

  std::string formChangeMessage(const std::string &paramName,
                                const std::string &valueStr,
                                bool changed = true) const;

  void handleLocationUpdate();
  void handleHandoverResponse(std::unique_ptr<common::Request> response);

  void handleActiveCommand(const MenuItemActive &cmd);
  void handleMoveCommand(const MenuItemMove<> &cmd);
  void handleProtocolCommand(const MenuItemProtocol &cmd);

  void handleSmsCommand(const MenuItemSMS &cmd);
  void addSentSms(const common::msisdn_t &targetMsisdn,
                  const std::string &smsContent, unsigned int smsId);

  void handleDialogCommand(const MenuItemDialog &cmd) const;
  void handleReceivedCommand() const;
  void handleSentCommand() const;

  void addDeliveryAckToExchange(const common::msisdn_t &msisdn,
                                unsigned int smsId);

  void exitApp();

  unsigned int generateSmsId();

  std::optional<common::msisdn_t> findBySpeedDialNum(char num);

  void logCommandProcess(std::string_view commandName,
                         std::string_view argsStr = "") const;

  void addMsg(const std::string &content,
              common::MenuMessageType type = common::MenuMessageType::INFO);

  void addErrorMsg(const std::string &content);

  void handleSmsInfoResponse(std::unique_ptr<common::Request> response);

public:
  App(const UeContext &ctx_,
      const std::map<char, common::msisdn_t> &addressBook_);

  void run();
};
} // namespace client
