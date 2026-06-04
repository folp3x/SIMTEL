#pragma once

#include "common/constants.h"
#include "common/logging/logger/logger.h"

namespace server {
template <typename T>
MenuItemDist<T>::MenuItemDist(const std::vector<T> &coords_)
    : coords(coords_) {}

template <typename T> std::string_view MenuItemDist<T>::getName() const {
  return "move";
}

template <typename T> size_t MenuItemDist<T>::getArgsCount() {
  return common::constants::LOCATION_COORDS_COUNT;
}

template <typename T>
MenuItemDist<T>::MenuItemDist(const MenuItemDist &other)
    : coords(other.coords) {
  logConstructor("COPY", coords);
}

template <typename T>
MenuItemDist<T>::MenuItemDist(MenuItemDist &&other) noexcept
    : coords(std::move(other.coords)) {
  logConstructor("MOVE", coords);
}

template <typename T>
void MenuItemDist<T>::logConstructor(const std::string &constructorType,
                                     const std::vector<T> &coords) const {
  SPDLOG_LOGGER_DEBUG(common::Logger::instance().getInner(),
                      "server::MenuItemDist {} constructor called: coords={}",
                      constructorType,
                      common::toStr(coords.begin(), coords.end()));
}

template <typename T> std::vector<T> MenuItemDist<T>::getCoords() const {
  return coords;
}
} // namespace server
