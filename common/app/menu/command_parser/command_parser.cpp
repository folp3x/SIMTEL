#include "command_parser.h"

#include "common/app/menu/menu_item/menu_item_exit/menu_item_exit.h"
#include "common/app/menu/menu_item/menu_item_invalid/menu_item_invalid.h"
#include "common/utils/str/str.h"

namespace common {
std::unique_ptr<MenuItem>
CommandParser::parseExitArgs(const std::vector<std::string> &args,
                             std::string &extraMsg) {
  if (args.size() > MenuItemExit::getArgsCount())
    extraMsg = "Extra arguments ignored";

  return std::make_unique<MenuItemExit>();
}

// парсит команду и ее аргументы
std::unique_ptr<MenuItem>
CommandParser::parseCommand(const std::string &str,
                            std::string &extraMsg) const {
  std::vector<std::string> tokens = split(lowercased(ltrimmed(str)));

  if (tokens.empty())
    return std::make_unique<MenuItemInvalid>("Empty command");

  std::string commandName = tokens[0];

  auto argsParsers = getArgsParsers();
  auto parserIt = argsParsers.find(commandName);
  if (parserIt == argsParsers.end()) {
    // если для команды нет обработчика
    return std::make_unique<MenuItemInvalid>("Unknown command");
  }

  tokens.erase(tokens.begin());

  auto cmd = parserIt->second(tokens, extraMsg);
  return cmd;
}
} // namespace common
