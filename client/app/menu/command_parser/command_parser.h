#pragma once

#include "common/app/menu/command_parser/command_parser.h"

#include "client/app/menu/command_info/command_info.h"

namespace client {
class CommandParser : public common::CommandParser {
private:
  const common::ArgsParsersMap argsParsers = {
      {"exit", common::CommandParser::parseExitArgs},
      {"active", parseActiveArgs},
      {"move", parseMoveArgs},
      {"protocol", parseProtocolArgs}};

  virtual common::ArgsParsersMap getArgsParsers() const;

  static std::unique_ptr<common::MenuItem>
  parseActiveArgs(const std::vector<std::string> &args, std::string &extraMsg);
  static std::unique_ptr<common::MenuItem>
  parseMoveArgs(const std::vector<std::string> &args, std::string &extraMsg);
  static std::unique_ptr<common::MenuItem>
  parseProtocolArgs(const std::vector<std::string> &args,
                    std::string &extraMsg);
};
} // namespace client
