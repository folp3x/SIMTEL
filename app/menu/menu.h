#ifndef MENU_H
#define MENU_H

#include <string>

#include "app/app_state.h"
#include "command_parser.h"
#include "core/location.h"
#include "core/protocol.h"

class Menu {
private:
  const int HEADER_LENGTH = 60;
  CommandParser parser;

public:
  Command getCommand();
  void showStatus(AppState state, const std::string &imsi,
                 const Location &location, Protocol protocol) const;
  void showMessage(const std::string &message) const;
  void showCommandsInfo() const;
};

#endif // MENU_H
