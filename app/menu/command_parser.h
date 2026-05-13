#pragma once

#include <memory>

#include "command_info.h"
#include "menu_item.h"

class CommandParser {
private:
  std::unique_ptr<MenuItem> parseExitArgs(std::istringstream &stream) const;
  std::unique_ptr<MenuItem> parseActiveArgs(std::istringstream &stream) const;
  std::unique_ptr<MenuItem> parseMoveArgs(std::istringstream &stream) const;
  std::unique_ptr<MenuItem> parseProtocolArgs(std::istringstream &stream) const;

public:
  std::unique_ptr<MenuItem> parseCommand(const std::string &str) const;
};
