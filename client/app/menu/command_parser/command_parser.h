#pragma once

#include "common/app/menu/command_parser/command_parser.h"

#include <functional>
#include <memory>
#include <unordered_map>

#include "client/app/menu/command_info/command_info.h"
#include "client/app/menu/menu_item/menu_item.h"

namespace client {
class CommandParser : public common::CommandParser {
  using ArgsParserMap =
      std::unordered_map<std::string,
                         std::function<std::unique_ptr<common::MenuItem>(
                             std::istringstream &, std::string &)>>;

private:
  const ArgsParserMap argsParsers = {
      {"exit", [this](std::istringstream &s,
                      std::string &m) { return parseExitArgs(s, m); }},
      {"active", [this](std::istringstream &s,
                        std::string &m) { return parseActiveArgs(s, m); }},
      {"move", [this](std::istringstream &s,
                      std::string &m) { return parseMoveArgs(s, m); }},
      {"protocol", [this](std::istringstream &s, std::string &m) {
         return parseProtocolArgs(s, m);
       }}};

  std::unique_ptr<common::MenuItem> parseExitArgs(std::istringstream &stream,
                                                  std::string &extraMsg) const;
  std::unique_ptr<common::MenuItem>
  parseActiveArgs(std::istringstream &stream, std::string &extraMsg) const;
  std::unique_ptr<common::MenuItem> parseMoveArgs(std::istringstream &stream,
                                                  std::string &extraMsg) const;
  std::unique_ptr<common::MenuItem>
  parseProtocolArgs(std::istringstream &stream, std::string &extraMsg) const;

public:
  virtual std::unique_ptr<common::MenuItem>
  parseCommand(const std::string &str, std::string &extraMsg) const override;
};
} // namespace client
