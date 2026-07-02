#include "menu.h"

#include <iostream>

#include "common/utils/print/print.h"

namespace common {
void Menu::showMessages(std::queue<MenuMessage> &messages) const {
  while (!messages.empty()) {
    showMessage(messages.front());
    messages.pop();
  }
}
} // namespace common
