#include "menu_item_exit.h"

namespace common {
std::string_view MenuItemExit::getName() const { return "exit"; }

int MenuItemExit::getArgsCount() { return 0; }
} // namespace common
