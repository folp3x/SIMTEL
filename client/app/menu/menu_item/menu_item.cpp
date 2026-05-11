#include "menu_item.h"

#include "common/constants/constants.h"

namespace client {
// MenuItemInvalid
MenuItemInvalid::MenuItemInvalid(const std::string &error_) : error(error_) {}

std::string_view MenuItemInvalid::getName() const { return "invalid"; }

std::string MenuItemInvalid::getError() const { return error; }

// MenuItemUnknown
std::string_view MenuItemUnknown::getName() const { return "unknown"; }

// MenuItemExit
std::string_view MenuItemExit::getName() const { return "exit"; }

int MenuItemExit::getArgsCount() { return 0; }

// MenuItemActive
MenuItemActive::MenuItemActive(bool active_) : active(active_) {}

std::string_view MenuItemActive::getName() const { return "active"; }

int MenuItemActive::getArgsCount() { return 1; }

bool MenuItemActive::getActive() const { return active; }

// MenuItemMove
MenuItemMove::MenuItemMove(const std::vector<float> &coords_)
    : coords(coords_) {}

std::string_view MenuItemMove::getName() const { return "move"; }

int MenuItemMove::getArgsCount() {
  return common::constants::LOCATION_COORDS_COUNT;
}

std::vector<float> MenuItemMove::getCoords() const { return coords; }

// MenuItemProtocol
MenuItemProtocol::MenuItemProtocol(const std::string &protocol_)
    : protocol(protocol_) {}

std::string_view MenuItemProtocol::getName() const { return "protocol"; }

int MenuItemProtocol::getArgsCount() { return 1; }

std::string MenuItemProtocol::getProtocol() const { return protocol; }
} // namespace client
