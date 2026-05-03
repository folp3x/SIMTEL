#pragma once

#include "app_state.h"
#include "config/config.h"
#include "core/location.h"
#include "core/network_address.h"
#include "core/protocol.h"
#include "menu/menu_item.h"

class App {
private:
  AppState state = AppState::INACTIVE;
  std::string imsi = "";
  Location location{{0, 0, 0}};
  Protocol protocol = Protocol::JSON;

  NetworkAddress addr{"127.0.0.1:49152"};
  std::string imei = "";

  std::string handleActiveCommand(const MenuItemActive &cmd);
  std::string handleMoveCommand(const MenuItemMove &cmd);
  std::string handleProtocolCommand(const MenuItemProtocol &cmd);

public:
  explicit App(const Config &config);
  App(const std::string &imsi_, const Location &location_,
      const NetworkAddress &addr_, const std::string imei_);

  void run();
};
