#pragma once

namespace common {
void Menu::showMenuHeaderLine() const {
  std::cout << std::string(MENU_HEADER_LINE_LENGTH, '-') << std::endl;
}

void Menu::showMessage(const MenuMessage &msg) const {
  printColored(msg.content, menuMessageTypeToColor(msg.type));
}
} // namespace common
