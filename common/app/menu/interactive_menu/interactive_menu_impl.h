#pragma once

namespace common {
template <std::derived_from<CommandParser> T>
std::unique_ptr<MenuItem>
InteractiveMenu<T>::getCommand(std::string &extraMsg) const {
  std::string input;
  std::cout << "> ";
  std::getline(std::cin, input, '\n');

  return parser.parseCommand(input, extraMsg);
}

template <std::derived_from<CommandParser> T>
void InteractiveMenu<T>::showCommandsInfo(
    const CommandInfoMap &commands) const {
  for (const auto &[_, info] : commands) {
    std::cout << "- " << info.usage << " - " << info.description << std::endl;
  }
}
} // namespace common
