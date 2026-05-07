#include "menu_item.h"

namespace client {
// MenuItemInvalid
MenuItemInvalid::MenuItemInvalid(const std::string &error_) : error(error_) {}

std::string MenuItemInvalid::getName() const { return "invalid"; }

std::string MenuItemInvalid::getError() const { return error; }

// MenuItemUnknown
std::string MenuItemUnknown::getName() const { return "unknown"; }

// MenuItemExit
std::string MenuItemExit::getName() const { return "exit"; }

// MenuItemActive
MenuItemActive::MenuItemActive(bool active_) : active(active_) {}

std::string MenuItemActive::getName() const { return "active"; }

bool MenuItemActive::getActive() const { return active; }

// MenuItemMove
MenuItemMove::MenuItemMove(const std::vector<double> &coords_)
    : coords(coords_) {}

std::string MenuItemMove::getName() const { return "move"; }

std::vector<double> MenuItemMove::getCoords() const { return coords; }

// MenuItemProtocol
MenuItemProtocol::MenuItemProtocol(const std::string &protocol_)
    : protocol(protocol_) {}

std::string MenuItemProtocol::getName() const { return "protocol"; }

std::string MenuItemProtocol::getProtocol() const { return protocol; }
} // namespace client
