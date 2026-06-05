#pragma once

#include "common/app/app/app.h"

#include <mutex>
#include <optional>
#include <queue>

#include "client/app/app_active/app_active.h"
#include "client/app/config/config/config.h"
#include "client/app/menu/menu/menu.h"
#include "client/app/menu/menu_item/menu_item_active/menu_item_active.h"
#include "client/app/menu/menu_item/menu_item_move/menu_item_move.h"
#include "client/app/menu/menu_item/menu_item_protocol/menu_item_protocol.h"
#include "client/app/menu/menu_item/menu_item_sms/menu_item_sms.h"
#include "common/network/network_address/network_address.h"
#include "common/network/protocol/protocol.h"

namespace client {
class App : common::App<Config> {
private:
  Menu menu;

  bool isRunning = false;

  const common::imsi_t imsi;
  const common::imei_t imei;

  bool inActive = false;
  common::Protocol protocol = common::Protocol::JSON;

  std::optional<float> distance = std::nullopt;
  std::mutex distanceMtx{};

  common::NetworkAddress serverAddr;

  std::priority_queue<common::MenuMessage> messages{};

  std::map<char, common::msisdn_t> addressBook{};

  common::MenuMessage formChangeMessage(const std::string &paramName,
                                        const std::string &valueStr,
                                        bool changed = true) const;

  std::expected<float, std::string> fetchDistance();

  void handleActiveCommand(const MenuItemActive &cmd);
  void handleMoveCommand(const MenuItemMove<> &cmd);
  void handleProtocolCommand(const MenuItemProtocol &cmd);
  void handleSmsCommand(const MenuItemSMS &cmd);

  virtual void handleCommand(const std::unique_ptr<common::MenuItem> &cmd,
                             bool &exit) override;

  void updateDistance(int updateFreqSec);

  std::optional<common::msisdn_t> findBySpeedDialNum(char num);

public:
  App(const common::Location<> &location_, const common::imsi_t &imsi_,
      const common::imei_t &imei_, const common::NetworkAddress &serverAddr_,
      const std::map<char, common::msisdn_t> &addressBook_);

  virtual void run() override;
};
} // namespace client
