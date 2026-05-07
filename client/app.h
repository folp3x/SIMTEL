#pragma once

#include <memory>

#include "common/app/app.h"

#include "app_state.h"
#include "common/core/location.h"
#include "common/core/network_address.h"
#include "common/core/protocol.h"
#include "config/config.h"
#include "menu/menu_item.h"

namespace client {
class App : common::App<Config> {
private:
  AppState state = AppState::INACTIVE;
  std::string imsi = "";
  std::string imei = "";

  std::string handleActiveCommand(const MenuItemActive &cmd);
  std::string handleMoveCommand(const MenuItemMove &cmd);
  std::string handleProtocolCommand(const MenuItemProtocol &cmd);

  virtual std::string
  handleCommand(const std::unique_ptr<common::MenuItem> &cmd,
                bool &exit) override;

public:
  App(const std::string &imsi_, const common::Location &location_,
      const common::NetworkAddress &addr_, const std::string imei_);

  virtual void run() override;
};
} // namespace client
