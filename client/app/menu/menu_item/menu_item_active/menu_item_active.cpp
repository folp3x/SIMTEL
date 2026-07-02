#include "menu_item_active.h"

namespace client {
MenuItemActive::MenuItemActive(bool active_) : active(active_) {}

std::string_view MenuItemActive::getName() const { return "active"; }

size_t MenuItemActive::getArgsCount() { return 1; }

bool MenuItemActive::getActive() const { return active; }
} // namespace client
