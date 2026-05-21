#pragma once

#include <ranges>

#include "common/core/location/location/location.h"

namespace server {
class DistanceCalculator {
public:
  template <typename Container>
    requires std::ranges::input_range<Container> &&
             std::ranges::sized_range<Container>
  static float calc(const common::Location<> &loc, const Container &coords);

  static float calc(const common::Location<> &loc1,
                    const common::coords_t<> &coords);

  static float calc(const common::Location<> &loc1,
                    const common::Location<> &loc2);
};
} // namespace server

#include "distance_calculator_impl.h"
