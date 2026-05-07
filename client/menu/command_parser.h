#pragma once

#include "common/app/menu/command_parser.h"

#include <memory>

#include "command_info.h"
#include "menu_item.h"

namespace client {
class CommandParser : public common::CommandParser {
private:
  std::unique_ptr<common::MenuItem>
  parseExitArgs(std::istringstream &stream) const;
  std::unique_ptr<common::MenuItem>
  parseActiveArgs(std::istringstream &stream) const;
  std::unique_ptr<common::MenuItem>
  parseMoveArgs(std::istringstream &stream) const;
  std::unique_ptr<common::MenuItem>
  parseProtocolArgs(std::istringstream &stream) const;

public:
  virtual std::unique_ptr<common::MenuItem>
  parseCommand(const std::string &str) const override;
};
} // namespace client
