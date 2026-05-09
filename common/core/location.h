#pragma once

#include <array>
#include <string>
#include <vector>

#include "common/constants.h"
#include "common/types.h"

namespace common {
class Location {
private:
  common::coords_t<float> coords = {0, 0, 0};

public:
  explicit Location(const common::coords_t<float> &coords);

  void move(const std::vector<float> &newCoords);
  void move(const common::coords_t<float> &newCoords);
  std::string toStr() const;
  bool coordsEqual(const std::vector<float> &otherCoords) const;

  common::coords_t<float> getCoords() const;
};
} // namespace common
