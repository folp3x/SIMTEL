#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include "menu_item_invalid.h"

#include <spdlog/spdlog.h>

namespace common {
MenuItemInvalid::MenuItemInvalid(const std::string &error_) : error(error_) {}

std::string_view MenuItemInvalid::getName() const { return ""; }

MenuItemInvalid::MenuItemInvalid(const MenuItemInvalid &other)
    : error(other.error) {
  logConstructor("COPY", error);
}

MenuItemInvalid::MenuItemInvalid(MenuItemInvalid &&other) noexcept
    : error(std::move(other.error)) {
  logConstructor("MOVE", error);
}

void MenuItemInvalid::logConstructor(const std::string &constructorType,
                                     const std::string &error) const {
  SPDLOG_LOGGER_DEBUG(spdlog::default_logger(),
                      "client::MenuItemInvalid {} constructor called: error={}",
                      constructorType, error);
}

std::string MenuItemInvalid::getError() const { return error; }
} // namespace common
