#pragma once

#include <concepts>
#include <iostream>
#include <spdlog/spdlog.h>
#include <string>

#include "command_parser.h"
#include "common/utils/print.h"

namespace common {
// базовый класс для вывода меню и получения команд
template <std::derived_from<CommandParser> T> class Menu {
private:
  static constexpr int HEADER_LENGTH = 60;
  T parser{};

public:
  virtual ~Menu() = default;

  inline void showHeaderLine() const {
    std::cout << std::string(HEADER_LENGTH, '-') << std::endl;
  }

  // получает команду от пользователя
  std::unique_ptr<MenuItem> getCommand() const {
    std::string input;
    std::cout << "> ";
    std::getline(std::cin, input, '\n');

    SPDLOG_LOGGER_INFO(spdlog::default_logger(), "Received input: {}", input);

    return parser.parseCommand(input);
  }

  // выводит сообщение
  void showMessage(const std::string &message) const {
    printlnColored(message, rang::fg::cyan);
  }
};
} // namespace common
