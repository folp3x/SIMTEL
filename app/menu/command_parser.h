#pragma once

#include "command.h"
#include "command_info.h"

class CommandParser {
private:
  Command parseExitArgs(std::istringstream &stream) const;
  Command parseActiveArgs(std::istringstream &stream) const;
  Command parseMoveArgs(std::istringstream &stream) const;
  Command parseProtocolArgs(std::istringstream &stream) const;

public:
  Command parseCommand(const std::string &str) const;
};
