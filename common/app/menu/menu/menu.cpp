#include "menu.h"

#include <iostream>

#include "common/utils/print/print.h"

namespace common {
void Menu::showMessages(
    std::priority_queue<common::MenuMessage> &messages) const {
  while (!messages.empty()) {
    showMessage(messages.top());
    messages.pop();
  }
}
} // namespace common
