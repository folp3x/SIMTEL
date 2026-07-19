#pragma once

namespace common {
void Menu::showMenuHeaderLine() const {
  std::cout << std::string(MenuHeaderLineLength, '-') << std::endl;
}

void Menu::showMessage(const MenuMessage &msg) const {
  utils::printColored(msg.content, utils::menuMessageTypeToColor(msg.type));
}
} // namespace common
