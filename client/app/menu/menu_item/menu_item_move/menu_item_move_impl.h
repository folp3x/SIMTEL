#include "menu_item_move.h"

#include "common/constants.h"

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
std::vector<T> MenuItemMove<T>::getCoords() const {
  return coords;
}
} // namespace client
