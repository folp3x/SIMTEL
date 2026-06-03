#include "menu_item_protocol.h"

#include "common/logging/logger/logger.h"

namespace client {
MenuItemProtocol::MenuItemProtocol(const std::string &protocol_)
    : protocol(protocol_) {}

std::string_view MenuItemProtocol::getName() const { return "protocol"; }

size_t MenuItemProtocol::getArgsCount() { return 1; }

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
      common::Logger::instance().getInner(),
      "client::MenuItemProtocol {} constructor called: protocol={}",
      constructorType, protocol);
}

std::string MenuItemProtocol::getProtocol() const { return protocol; }
} // namespace client
