#pragma once

#include <concepts>
#include <queue>
#include <string>

#include "common/app/menu/command_parser/command_parser.h"
#include "common/app/menu/menu_message/menu_message.h"

namespace common {
// базовый класс для вывода меню и получения команд
template <std::derived_from<CommandParser> T> class Menu {
private:
  static constexpr int MENU_HEADER_LINE_LENGTH = 60;
  T parser{};

protected:
  virtual void logInput(const std::string &input) const = 0;

public:
  virtual ~Menu() = default;

  inline void showMenuHeaderLine() const;

  std::unique_ptr<MenuItem> getCommand(std::string &extraMsg) const;

  inline void showMessage(const MenuMessage &msg) const;
  void showMessages(std::priority_queue<common::MenuMessage> &messages) const;

  void showCommandsInfo(const CommandInfoMap &commands) const;
};
} // namespace common

#include "menu_impl.h"
