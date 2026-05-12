#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include "menu_item_active.h"

#include <spdlog/spdlog.h>

namespace client {
MenuItemActive::MenuItemActive(bool active_) : active(active_) {}

std::string_view MenuItemActive::getName() const { return "active"; }

int MenuItemActive::getArgsCount() { return 1; }

MenuItemActive::MenuItemActive(const MenuItemActive &other)
    : active(other.active) {
  logConstructor("COPY", active);
}

MenuItemActive::MenuItemActive(MenuItemActive &&other) noexcept
    : active(other.active) {
  logConstructor("MOVE", active);
}

void MenuItemActive::logConstructor(const std::string &constructorType,
                                    bool active) const {
  SPDLOG_LOGGER_DEBUG(spdlog::default_logger(),
                      "client::MenuItemActive {} constructor called: active={}",
                      constructorType, active);
}

bool MenuItemActive::getActive() const { return active; }
} // namespace client
