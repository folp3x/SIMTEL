#include "menu.h"

#include <iostream>
#include <spdlog/spdlog.h>

#include "utils/print.h"

// получает команду от пользоватеоя
std::unique_ptr<MenuItem> Menu::getCommand() {
  std::string input;
  std::cout << "> ";
  std::getline(std::cin, input, '\n');

  SPDLOG_LOGGER_INFO(spdlog::default_logger(), "Received input: {}", input);

  return parser.parseCommand(input);
}

// выводит текущий статус
void Menu::showStatus(AppState state, const std::string &imsi,
                      const Location &location, Protocol protocol) const {
  std::cout << std::string(HEADER_LENGTH, '-') << std::endl;

  std::cout << "IMSI: " << imsi << std::endl;

  // текущее состояние
  std::cout << "State: ";
  std::string statusStr = appStateToStr(state);
  switch (state) {
  case AppState::ACTIVE:
    printlnColored(statusStr, rang::fg::green);
    break;
  case AppState::INACTIVE:
    printlnColored(statusStr, rang::fg::red);
    break;
  }

  std::cout << "Location: " << location.toStr() << std::endl;
  std::cout << "Protocol: " << protocolToStr(protocol) << std::endl;

  std::cout << std::string(HEADER_LENGTH, '-') << std::endl;
}

// выводит сообщение
void Menu::showMessage(const std::string &message) const {
  printlnColored(message, rang::fg::cyan);
}

// выводит информацию о доутспных командах
void Menu::showCommandsInfo() const {
  const auto &commands = getCommandsInfo();
  for (const auto &[_, info] : commands) {
    std::cout << "- " << info.usage << " - " << info.description << std::endl;
  }
}
