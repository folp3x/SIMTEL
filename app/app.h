#pragma once

#include "app_state.h"
#include "config/config.h"
#include "core/location.h"
#include "core/protocol.h"
#include "menu/menu_item.h"

class App {
private:
  AppState state = AppState::INACTIVE;
  std::string imsi = "";
  Location location;
  Protocol protocol = Protocol::JSON;

  std::string ip = "";
  int port = 0;
  std::string imei = "";

  std::string handleActiveCommand(const MenuItemActive &cmd);
  std::string handleMoveCommand(const MenuItemMove &cmd);
  std::string handleProtocolCommand(const MenuItemProtocol &cmd);

public:
  App(const Config &config);

  void run();
};
