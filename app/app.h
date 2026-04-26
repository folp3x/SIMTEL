#ifndef APP_H
#define APP_H

#include "app_state.h"
#include "core/location.h"
#include "core/protocol.h"
#include "menu/command.h"

class App {
private:
  AppState state = AppState::INACTIVE;
  std::string imsi = "";
  Location location;
  Protocol protocol = Protocol::JSON;

  std::string handleActiveCommand(const ActiveCommand &cmd);
  std::string handleMoveCommand(const MoveCommand &cmd);
  std::string handleProtocolCommand(const ProtocolCommand &cmd);

public:
  void run();
};

#endif // APP_H
