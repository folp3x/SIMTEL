#ifndef APP_H
#define APP_H

#include "app_state.h"
#include "config/config.h"
#include "core/location.h"
#include "core/protocol.h"
#include "menu/command.h"

class App {
private:
  AppState state = AppState::INACTIVE;
  std::string imsi = "";
  Location location;
  Protocol protocol = Protocol::JSON;

  std::string ip = "";
  int port = 0;
  std::string imei = "";

  std::string handleActiveCommand(const ActiveCommand &cmd);
  std::string handleMoveCommand(const MoveCommand &cmd);
  std::string handleProtocolCommand(const ProtocolCommand &cmd);

public:
  App(const Config &config);

  void run();
};

#endif // APP_H
