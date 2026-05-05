#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

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

#endif // COMMAND_PARSER_H
