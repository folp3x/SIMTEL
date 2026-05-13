#pragma once

#include <concepts>
#include <iostream>
#include <string>

#include "common/app/menu/command_parser/command_parser.h"
#include "common/utils/print/print.h"

namespace common {
// базовый класс для вывода меню и получения команд
template <std::derived_from<CommandParser> T> class Menu {
private:
  static constexpr int MENU_HEADER_LINE_LENGTH = 60;
  T parser{};

protected:
  virtual void logInput(const std::string &input) const {};

public:
  virtual ~Menu() = default;

  inline void showHeaderLine() const {
    std::cout << std::string(MENU_HEADER_LINE_LENGTH, '-') << std::endl;
  }

  // получает команду от пользователя
  std::unique_ptr<MenuItem> getCommand(std::string &extraMsg) const {
    std::string input;
    std::cout << "> ";
    std::getline(std::cin, input, '\n');

    logInput(input);

    return parser.parseCommand(input, extraMsg);
  }

  // выводит сообщение
  void showMessage(std::string_view message) const {
    printColored(message, rang::fg::cyan);
  }

  // выводит информацию о доступных командах
  void showCommandsInfo(const CommandInfoMap &commands) const {
    for (const auto &[_, info] : commands)
      std::cout << "- " << info.usage << " - " << info.description << std::endl;
  }
};
} // namespace common
