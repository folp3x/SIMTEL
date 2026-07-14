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
  static constexpr char SpeedDialNumPrefix = '@';

  static constexpr char UssdPrefix = '*';
  static constexpr char UssdPostfix = '#';

  static constexpr size_t UssdPrefixLength = 1;
  static constexpr size_t UssdPostfixLength = 1;

  const CommandParser::arg_parser_map_t argsParsers = {
      {"exit", parseExitArgs},
      {"active", parseActiveArgs},
      {"move", parseMoveArgs<>},
      {"protocol", parseProtocolArgs},
      {"sms", parseSmsArgs},
      {"sent", parseWithoutArgs<MenuItemSent>},
      {"received", parseWithoutArgs<MenuItemReceived>},
      {"dialog", parseDialogArgs},
      {"ussd", parseWithoutArgs<MenuItemUssd>}};

  virtual CommandParser::arg_parser_map_t getArgsParsers() const override;

  template <typename T = float>
  static std::unique_ptr<common::MenuItem>
  parseMoveArgs(const std::string &initialStr,
                const std::vector<std::string> &args, std::string &extraMsg);

  template <typename T>
  static std::unique_ptr<common::MenuItem>
  parseWithoutArgs(const std::string &initialStr,
                   const std::vector<std::string> &args, std::string &extraMsg);

  static std::unique_ptr<common::MenuItem>
  parseProtocolArgs(const std::string &initialStr,
                    const std::vector<std::string> &args,
                    std::string &extraMsg);

  static std::unique_ptr<common::MenuItem>
  parseSmsArgs(const std::string &initialStr,
               const std::vector<std::string> &args, std::string &extraMsg);

  static std::unique_ptr<common::MenuItem>
  parseDialogArgs(const std::string &initialStr,
                  const std::vector<std::string> &args, std::string &extraMsg);

  static std::unique_ptr<common::MenuItem>
  parseExitArgs(const std::string &initialStr,
                const std::vector<std::string> &args, std::string &extraMsg);

  static std::unique_ptr<common::MenuItem>
  parseActiveArgs(const std::string &initialStr,
                  const std::vector<std::string> &args, std::string &extraMsg);

  static std::unique_ptr<common::MenuItem>
  parseUssdCodeArgs(const std::string &initialStr,
                    const std::vector<std::string> &args,
                    std::string &extraMsg);

public:
  virtual std::unique_ptr<common::MenuItem>
  parseCommand(const std::string &str, std::string &extraMsg) const override;
};
} // namespace client

#include "command_parser_impl.h"
