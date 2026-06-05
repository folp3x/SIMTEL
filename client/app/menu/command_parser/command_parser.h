#pragma once

#include "common/app/menu/command_parser/command_parser.h"

#include "client/app/menu/command_info/command_info.h"
#include "client/app/menu/menu_item/menu_item_received/menu_item_received.h"
#include "client/app/menu/menu_item/menu_item_sent/menu_item_sent.h"

namespace client {
class CommandParser : public common::CommandParser {
private:
  const CommandParser::ArgsParsersMap argsParsers = {
      {"exit", common::CommandParser::parseExitArgs},
      {"active", parseActiveArgs},
      {"move", parseMoveArgs<>},
      {"protocol", parseProtocolArgs},
      {"sms", parseSmsArgs},
      {"sent", parseWithoutArgs<MenuItemSent>},
      {"received", parseWithoutArgs<MenuItemReceived>},
      {"dialog", parseDialogArgs}};

  virtual CommandParser::ArgsParsersMap getArgsParsers() const override;

  static std::unique_ptr<common::MenuItem>
  parseActiveArgs(const std::vector<std::string> &args, std::string &extraMsg);

  template <typename T = float>
  static std::unique_ptr<common::MenuItem>
  parseMoveArgs(const std::vector<std::string> &args, std::string &extraMsg);

  static std::unique_ptr<common::MenuItem>
  parseProtocolArgs(const std::vector<std::string> &args,
                    std::string &extraMsg);

  static std::unique_ptr<common::MenuItem>
  parseSmsArgs(const std::vector<std::string> &args, std::string &extraMsg);

  static std::unique_ptr<common::MenuItem>
  parseDialogArgs(const std::vector<std::string> &args, std::string &extraMsg);

  template <typename T>
  static std::unique_ptr<common::MenuItem>
  parseWithoutArgs(const std::vector<std::string> &args, std::string &extraMsg);
};
} // namespace client

#include "command_parser_impl.h"
