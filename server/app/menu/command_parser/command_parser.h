#pragma once

#include "common/app/menu/command_parser/command_parser.h"

#include "server/app/menu/command_info/command_info.h"

namespace server {
class CommandParser : public common::CommandParser {
private:
  const common::ArgsParsersMap argsParsers = {
      {"exit", common::CommandParser::parseExitArgs}, {"dist", parseDistArgs}};

  common::ArgsParsersMap getArgsParsers() const;

  static std::unique_ptr<common::MenuItem>
  parseDistArgs(const std::vector<std::string> &args, std::string &extraMsg);
};
} // namespace server
