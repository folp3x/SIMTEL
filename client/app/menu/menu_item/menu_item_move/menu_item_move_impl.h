#include "menu_item_move.h"

#include "common/constants.h"
#include "common/logging/logger/logger.h"
#include "common/utils/str/str.h"

namespace client {
template <typename T>
  requires std::is_arithmetic_v<T>
MenuItemMove<T>::MenuItemMove(const std::vector<T> &coords_)
    : coords(coords_) {}

template <typename T>
  requires std::is_arithmetic_v<T>
std::string_view MenuItemMove<T>::getName() const {
  return "move";
}

template <typename T>
  requires std::is_arithmetic_v<T>
size_t MenuItemMove<T>::getArgsCount() {
  return common::constants::LOCATION_COORDS_COUNT;
}

template <typename T>
  requires std::is_arithmetic_v<T>
MenuItemMove<T>::MenuItemMove(const MenuItemMove &other)
    : coords(other.coords) {
  logConstructor("COPY", coords);
}

template <typename T>
  requires std::is_arithmetic_v<T>
MenuItemMove<T>::MenuItemMove(MenuItemMove &&other) noexcept
    : coords(std::move(other.coords)) {
  logConstructor("MOVE", coords);
}

template <typename T>
  requires std::is_arithmetic_v<T>
void MenuItemMove<T>::logConstructor(const std::string &constructorType,
                                     const std::vector<T> &coords) const {
  SPDLOG_LOGGER_DEBUG(common::Logger::instance().getInner(),
                      "client::MenuItemMove {} constructor called: coords={}",
                      constructorType,
                      common::toStr(coords.begin(), coords.end()));
}

template <typename T>
  requires std::is_arithmetic_v<T>
std::vector<T> MenuItemMove<T>::getCoords() const {
  return coords;
}
} // namespace client
