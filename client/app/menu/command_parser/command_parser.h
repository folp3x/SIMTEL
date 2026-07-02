#pragma once

#include "common/app/menu/command_parser/command_parser.h"

#include "client/app/menu/menu_item/menu_item_exit/menu_item_exit.h"
#include "client/app/menu/menu_item/menu_item_move/menu_item_move.h"
#include "client/app/menu/menu_item/menu_item_received/menu_item_received.h"
#include "client/app/menu/menu_item/menu_item_sent/menu_item_sent.h"
#include "client/app/menu/menu_item/menu_item_ussd/menu_item_ussd.h"
#include "common/app/menu/menu_item/menu_item_invalid/menu_item_invalid.h"
#include "common/utils/str/str.h"

namespace client {
class CommandParser : public common::CommandParser {
private:
  const CommandParser::ArgsParsersMap argsParsers = {
      {"exit", parseExitArgs},
      {"active", parseActiveArgs},
      {"move", parseMoveArgs<>},
      {"protocol", parseProtocolArgs},
      {"sms", parseSmsArgs},
      {"sent", parseWithoutArgs<MenuItemSent>},
      {"received", parseWithoutArgs<MenuItemReceived>},
      {"dialog", parseDialogArgs},
      {"ussd", parseWithoutArgs<MenuItemUssd>}};

  virtual CommandParser::ArgsParsersMap getArgsParsers() const override;

  template <typename T = float>
  static std::unique_ptr<common::MenuItem>
  parseMoveArgs(const std::vector<std::string> &args, std::string &extraMsg);

  template <typename T>
  static std::unique_ptr<common::MenuItem>
  parseWithoutArgs(const std::vector<std::string> &args, std::string &extraMsg);

  static std::unique_ptr<common::MenuItem>
  parseProtocolArgs(const std::vector<std::string> &args,
                    std::string &extraMsg);

  static std::unique_ptr<common::MenuItem>
  parseSmsArgs(const std::vector<std::string> &args, std::string &extraMsg);

  static std::unique_ptr<common::MenuItem>
  parseDialogArgs(const std::vector<std::string> &args, std::string &extraMsg);

  static std::unique_ptr<common::MenuItem>
  parseExitArgs(const std::vector<std::string> &args, std::string &extraMsg);

  static std::unique_ptr<common::MenuItem>
  parseActiveArgs(const std::vector<std::string> &args, std::string &extraMsg);

  static std::unique_ptr<common::MenuItem>
  parseUssdCodeArgs(const std::vector<std::string> &args,
                    std::string &extraMsg);

public:
  virtual std::unique_ptr<common::MenuItem>
  parseCommand(const std::string &str, std::string &extraMsg) const override;
};
} // namespace client

#include "command_parser_impl.h"
