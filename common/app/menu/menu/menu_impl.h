#pragma once

#include <iostream>

#include "common/utils/print/print.h"

namespace common {
template <std::derived_from<CommandParser> T>
void Menu<T>::showMenuHeaderLine() const {
  std::cout << std::string(MENU_HEADER_LINE_LENGTH, '-') << std::endl;
}

template <std::derived_from<CommandParser> T>
std::unique_ptr<MenuItem> Menu<T>::getCommand(std::string &extraMsg) const {
  std::string input;
  std::cout << "> ";
  std::getline(std::cin, input, '\n');

  logInput(input);

  return parser.parseCommand(input, extraMsg);
}

template <std::derived_from<CommandParser> T>
void Menu<T>::showMessage(const MenuMessage &msg) const {
  printColored(msg.content, menuMessageTypeToColor(msg.type));
}

template <std::derived_from<CommandParser> T>
void Menu<T>::showMessages(
    std::priority_queue<common::MenuMessage> &messages) const {
  while (!messages.empty()) {
    showMessage(messages.top());
    messages.pop();
  }
}

template <std::derived_from<CommandParser> T>
void Menu<T>::showCommandsInfo(const CommandInfoMap &commands) const {
  for (const auto &[_, info] : commands) {
    std::cout << "- " << info.usage << " - " << info.description << std::endl;
  }
}
} // namespace common
