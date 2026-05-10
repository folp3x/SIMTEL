#pragma once

#include <algorithm>
#include <array>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>

#include "common/constants/constants.h"
#include "common/types/types.h"

namespace common {
class Location {
private:
  common::coords_t<float> coords = {0, 0, 0};

public:
  explicit Location(const common::coords_t<float> &coords);

  // сдвигает текущие координаты в позицию newCoords
  // если newCoords содержит не все координаты изменяется только часть координат
  template <typename C>
    requires std::ranges::input_range<C> && std::ranges::sized_range<C>
  void move(const C &newCoords) {
    if (newCoords.size() > coords.size()) {
      throw std::invalid_argument(
          "newCoords size cant be bigger than " +
          std::to_string(coords.size()));
    }

    if (std::ranges::empty(newCoords)) {
      throw std::invalid_argument("newCoords cant be empty" +
                                  std::to_string(coords.size()));
    }

    std::ranges::copy(newCoords, coords.begin());
  }

  std::string toStr() const;
  bool coordsEqual(const std::vector<float> &otherCoords) const;

  common::coords_t<float> getCoords() const;
};
} // namespace common
