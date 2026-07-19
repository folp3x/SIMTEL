#include "menu_item_protocol.h"

namespace client {
MenuItemProtocol::MenuItemProtocol(const std::string &protocol_)
    : protocol(protocol_) {}

std::string_view MenuItemProtocol::getName() const { return "protocol"; }

size_t MenuItemProtocol::getArgsCount() { return 1; }

std::string MenuItemProtocol::getProtocol() const { return protocol; }
} // namespace client
