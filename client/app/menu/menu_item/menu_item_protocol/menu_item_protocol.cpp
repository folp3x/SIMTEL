#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include "menu_item_protocol.h"

#include <spdlog/spdlog.h>

namespace client {
MenuItemProtocol::MenuItemProtocol(const std::string &protocol_)
    : protocol(protocol_) {}

std::string_view MenuItemProtocol::getName() const { return "protocol"; }

int MenuItemProtocol::getArgsCount() { return 1; }

MenuItemProtocol::MenuItemProtocol(const MenuItemProtocol &other)
    : protocol(other.protocol) {
  logConstructor("COPY", protocol);
}

MenuItemProtocol::MenuItemProtocol(MenuItemProtocol &&other) noexcept
    : protocol(std::move(other.protocol)) {
  logConstructor("MOVE", protocol);
}

void MenuItemProtocol::logConstructor(const std::string &constructorType,
                                      const std::string &protocol) const {
  SPDLOG_LOGGER_DEBUG(
      spdlog::default_logger(),
      "client::MenuItemProtocol {} constructor called: protocol={}",
      constructorType, protocol);
}

std::string MenuItemProtocol::getProtocol() const { return protocol; }
} // namespace client
