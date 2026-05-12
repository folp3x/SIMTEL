#pragma once

#include <ranges>

#include "common/core/location/location.h"

namespace server {
class DistanceCalculator {
public:
  template <typename Container>
    requires std::ranges::input_range<Container> &&
             std::ranges::sized_range<Container>
  static float calc(const common::Location<float> &loc,
                    const Container &coords) {

    if (coords.size() != loc.getCoordsCount())
      throw std::invalid_argument(
          "coords size must be equal to loc coords size");

    // вычисление евклидова расстояния
    auto locCoords = loc.getCoords();
    float dist = 0;
    auto it1 = locCoords.begin();
    auto it2 = coords.begin();
    while (it1 != locCoords.end() || it2 != coords.end()) {
      dist += pow(*it1 - *it2, 2);
      ++it1;
      ++it2;
    }

    return sqrt(dist);
  }

  float calc(const common::Location<float> &loc1,
             const common::coords_t<float> &coords);

  float calc(const common::Location<float> &loc1,
             const common::Location<float> &loc2);
};
} // namespace server
