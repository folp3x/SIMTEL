#include "menu.h"

#include <iostream>

namespace server {
void Menu::showStatus(const common::Location<float> &location) const {
  showHeaderLine();
  std::cout << "Location: " << location.toStr() << std::endl;
  showHeaderLine();
}
} // namespace server
