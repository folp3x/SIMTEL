#pragma once

#include "common/app/menu/menu/menu.h"

#include <concepts>

#include "common/app/menu/command_info/command_info.h"
#include "common/app/menu/command_parser/command_parser.h"

namespace common {
// базовый класс для вывода меню и получения команд
template <std::derived_from<CommandParser> T>
class InteractiveMenu : public Menu {
private:
  T parser{};

  virtual void logInput(const std::string &input) const = 0;

public:
  std::unique_ptr<MenuItem> getCommand(std::string &extraMsg) const;

  void showCommandsInfo(const CommandInfoMap &commands) const;
};
} // namespace common

#include "interactive_menu_impl.h"
