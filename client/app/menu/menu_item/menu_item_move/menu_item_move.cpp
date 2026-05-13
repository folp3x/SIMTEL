#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include "menu_item_move.h"

#include <spdlog/spdlog.h>

#include "common/constants/constants.h"
#include "common/utils/str/str.h"

namespace client {
MenuItemMove::MenuItemMove(const std::vector<float> &coords_)
    : coords(coords_) {}

std::string_view MenuItemMove::getName() const { return "move"; }

int MenuItemMove::getArgsCount() {
  return common::constants::LOCATION_COORDS_COUNT;
}

MenuItemMove::MenuItemMove(const MenuItemMove &other) : coords(other.coords) {
  logConstructor("COPY", coords);
}

MenuItemMove::MenuItemMove(MenuItemMove &&other) noexcept
    : coords(std::move(other.coords)) {
  logConstructor("MOVE", coords);
}

void MenuItemMove::logConstructor(const std::string &constructorType,
                                  const std::vector<float> &coords) const {
  SPDLOG_LOGGER_DEBUG(spdlog::default_logger(),
                      "client::MenuItemMove {} constructor called: coords={}",
                      constructorType,
                      common::toStr(coords.begin(), coords.end()));
}

std::vector<float> MenuItemMove::getCoords() const { return coords; }
} // namespace client
