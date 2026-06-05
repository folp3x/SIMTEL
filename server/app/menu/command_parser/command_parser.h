#pragma once

#include "common/app/menu/command_parser/command_parser.h"

namespace server {
class CommandParser : public common::CommandParser {
private:
  const CommandParser::ArgsParsersMap argsParsers = {
      {"exit", common::CommandParser::parseExitArgs},
      {"dist", parseDistArgs<>}};

  CommandParser::ArgsParsersMap getArgsParsers() const;

  template <typename T = float>
  static std::unique_ptr<common::MenuItem>
  parseDistArgs(const std::vector<std::string> &args, std::string &extraMsg);
};
} // namespace server

#include "command_parser_impl.h"
