#include "menu.h"

#include "common/utils/print/print.h"

namespace server {
void Menu::showStatus() const { std::cout << "Server started" << std::endl; }
} // namespace server
