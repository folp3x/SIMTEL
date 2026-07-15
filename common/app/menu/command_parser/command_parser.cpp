#include "command_parser.h"

#include "common/app/menu/menu_item/menu_item_empty/menu_item_empty.h"
#include "common/app/menu/menu_item/menu_item_invalid/menu_item_invalid.h"
#include "common/utils/str/str.h"

namespace common {
// парсит команду и ее аргументы
std::unique_ptr<MenuItem>
CommandParser::parseCommand(const std::string &str,
                            std::string &extraMsg) const {
  std::vector<std::string> tokens = utils::split(str);

  if (tokens.empty()) {
    return std::make_unique<MenuItemEmpty>();
  }

  std::string commandName = utils::lowercased(tokens[0]);
  // удаление названия команды
  tokens.erase(tokens.begin());

  auto argsParsers = getArgsParsers();
  auto it = argsParsers.find(commandName);
  if (it == argsParsers.end()) {
    // если для команды нет обработчика
    return std::make_unique<MenuItemInvalid>("Unknown command '" + commandName +
                                             "'");
  }

  auto cmd = it->second(str, tokens, extraMsg);

  return cmd;
}
} // namespace common
