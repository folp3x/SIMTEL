#pragma once

#include "command.h"
#include "command_info.h"

class CommandParser {
private:
  Command parseExitArgs(std::istringstream &stream);
  Command parseActiveArgs(std::istringstream &stream);
  Command parseMoveArgs(std::istringstream &stream);
  Command parseProtocolArgs(std::istringstream &stream);

public:
  Command parseCommand(const std::string &str);
};
