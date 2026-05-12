#include "distance_calculator.h"

#include <stdexcept>

namespace server {
float DistanceCalculator::calc(const common::Location<float> &loc1,
                               const common::coords_t<float> &coords) {
  return calc<common::coords_t<float>>(loc1, coords);
}

float DistanceCalculator::calc(const common::Location<float> &loc1,
                               const common::Location<float> &loc2) {
  return calc(loc1, loc2.getCoords());
}
} // namespace server
