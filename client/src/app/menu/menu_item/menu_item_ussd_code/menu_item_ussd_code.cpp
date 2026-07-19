#include "menu_item_ussd_code.h"

namespace client {
MenuItemUssdCode::MenuItemUssdCode(unsigned int code_) : code(code_) {}

std::string_view MenuItemUssdCode::getName() const { return "*#"; }

unsigned int MenuItemUssdCode::getCode() const { return code; }
} // namespace client
