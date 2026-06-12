#pragma once

#include "common/app/app/app.h"

#include <optional>
#include <queue>

#include "client/app/config/config/config.h"
#include "client/app/menu/menu/menu.h"
#include "client/app/menu/menu_item/menu_item_active/menu_item_active.h"
#include "client/app/menu/menu_item/menu_item_move/menu_item_move.h"
#include "client/app/menu/menu_item/menu_item_protocol/menu_item_protocol.h"
#include "client/app/menu/menu_item/menu_item_sms/menu_item_sms.h"
#include "client/core/ue/ue_active/ue_active.h"
#include "client/core/ue/ue_context/ue_context.h"
#include "client/core/ue/ue_exchange/ue_exchange.h"
#include "common/core/request/rrc_connection_request/rrc_connection_request.h"

namespace client {
class App : common::App<Config> {
private:
  static constexpr unsigned int HANDOVER_SIGNAL_THRESHOLD = 40;

  UeContext ctx;
  UeExchange exchange;

  Menu menu;

  bool isRunning = false;

  std::priority_queue<common::MenuMessage> messages{};

  std::map<char, common::msisdn_t> addressBook{};

  void sigintHandler(int signal);

  std::string formChangeMessage(const std::string &paramName,
                                const std::string &valueStr,
                                bool changed = true) const;

  void handleLocationUpdate();

  void handleActiveCommand(const MenuItemActive &cmd);
  void handleMoveCommand(const MenuItemMove<> &cmd);
  void handleProtocolCommand(const MenuItemProtocol &cmd);
  void handleSmsCommand(const MenuItemSMS &cmd);

  virtual void handleCommand(const std::unique_ptr<common::MenuItem> &cmd,
                             bool &exit);

  std::optional<common::msisdn_t> findBySpeedDialNum(char num);

  void logCommandProcess(std::string_view commandName,
                         std::string_view argsStr = "") const;

  void addMsg(const std::string &content,
              common::MenuMessageType type = common::MenuMessageType::INFO);

  void addErrorMsg(const std::string &content);

public:
  App(const UeContext &ctx_,
      const std::map<char, common::msisdn_t> &addressBook_);

  virtual void run() override;
};
} // namespace client
