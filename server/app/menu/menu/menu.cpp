#include "menu.h"

#include "common/utils/print/print.h"

namespace server {
void Menu::showStatus() const {
  common::printColored("Server started", rang::fg::blue);
}
} // namespace server
