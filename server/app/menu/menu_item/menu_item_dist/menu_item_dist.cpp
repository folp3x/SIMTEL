#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include "menu_item_dist.h"

#include <spdlog/spdlog.h>

#include "common/constants/constants.h"
#include "common/utils/str/str.h"

namespace server {
MenuItemDist::MenuItemDist(const std::vector<float> &coords_)
    : coords(coords_) {}

std::string_view MenuItemDist::getName() const { return "move"; }

int MenuItemDist::getArgsCount() {
  return common::constants::LOCATION_COORDS_COUNT;
}

MenuItemDist::MenuItemDist(const MenuItemDist &other) : coords(other.coords) {
  logConstructor("COPY", coords);
}

MenuItemDist::MenuItemDist(MenuItemDist &&other) noexcept
    : coords(std::move(other.coords)) {
  logConstructor("MOVE", coords);
}

void MenuItemDist::logConstructor(const std::string &constructorType,
                                  const std::vector<float> &coords) const {
  SPDLOG_LOGGER_DEBUG(spdlog::default_logger(),
                      "server::MenuItemDist {} constructor called: coords={}",
                      constructorType,
                      common::toStr(coords.begin(), coords.end()));
}

std::vector<float> MenuItemDist::getCoords() const { return coords; }
} // namespace server
