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
                             const std::vector<std::string> &, std::string &)>>;

private:
  const ArgsParserMap argsParsers = {{"exit", parseExitArgs},
                                     {"active", parseActiveArgs},
                                     {"move", parseMoveArgs},
                                     {"protocol", parseProtocolArgs}};

  static std::unique_ptr<common::MenuItem>
  parseExitArgs(const std::vector<std::string> &args, std::string &extraMsg);
  static std::unique_ptr<common::MenuItem>
  parseActiveArgs(const std::vector<std::string> &args, std::string &extraMsg);
  static std::unique_ptr<common::MenuItem>
  parseMoveArgs(const std::vector<std::string> &args, std::string &extraMsg);
  static std::unique_ptr<common::MenuItem>
  parseProtocolArgs(const std::vector<std::string> &args,
                    std::string &extraMsg);

public:
  virtual std::unique_ptr<common::MenuItem>
  parseCommand(const std::string &str, std::string &extraMsg) const override;
};
} // namespace client
