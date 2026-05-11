#pragma once

#include "common/app/app/app.h"

#include <functional>
#include <memory>

#include "client/app/app_state/app_state.h"
#include "client/app/config/config/config.h"
#include "client/app/menu/menu_item/menu_item.h"

namespace client {
class App : common::App<Config> {
private:
  AppState state = AppState::INACTIVE;
  const std::string imsi = "";
  const std::string imei = "";

  std::string formChangeMessage(const std::string &paramName,
                                const std::string &valueStr,
                                bool changed = true) {
    std::string message = paramName;
    if (changed)
      message += " changed to " + valueStr;
    else
      message += " already set to " + valueStr;

    return message;
  }

  std::string handleActiveCommand(const MenuItemActive &cmd);
  std::string handleMoveCommand(const MenuItemMove &cmd);
  std::string handleProtocolCommand(const MenuItemProtocol &cmd);

  virtual std::string
  handleCommand(const std::unique_ptr<common::MenuItem> &cmd,
                bool &exit) override;

public:
  App(const common::Location &location_, const common::NetworkAddress &addr_,
      const common::imsi_t &imsi_, const common::imei_t imei_);

  virtual void run() override;
};
} // namespace client
