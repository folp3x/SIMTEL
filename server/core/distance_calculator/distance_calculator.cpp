#include "distance_calculator.h"

namespace server {
float DistanceCalculator::calc(const common::Location<> &loc1,
                               const common::coords_t<> &coords) {
  return calc<common::coords_t<>>(loc1, coords);
}

float DistanceCalculator::calc(const common::Location<> &loc1,
                               const common::Location<> &loc2) {
  return calc(loc1, loc2.getCoords());
}
} // namespace server
