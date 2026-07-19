#include "menu_item_ussd.h"

namespace client {
std::string_view MenuItemUssd::getName() const { return "ussd"; }

size_t MenuItemUssd::getArgsCount() { return 0; }
} // namespace client
