#include "menu_item_invalid.h"

namespace common {
MenuItemInvalid::MenuItemInvalid(const std::string &error_) : error(error_) {}

std::string_view MenuItemInvalid::getName() const { return ""; }

std::string MenuItemInvalid::getError() const { return error; }
} // namespace common
