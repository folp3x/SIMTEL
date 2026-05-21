#include "menu.h"

#include <iostream>
#include <rang.hpp>

namespace server {
void Menu::logInput(const std::string &input) const {};

void Menu::showStatus(const common::Location<> &location) const {
  std::cout << "Location: " << location.toStr() << std::endl;
  common::printColored("Processing clients...", rang::fg::cyan);
}
} // namespace server
