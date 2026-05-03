#pragma once

#include <array>
#include <string>
#include <vector>

#include "common/constants.h"

class Location {
private:
  std::array<double, Constants::LOCATION_COORDS_COUNT> coords = {0, 0, 0};

public:
  explicit Location(
      const std::array<double, Constants::LOCATION_COORDS_COUNT> &coords);

  void move(const std::vector<double> &newCoords);
  void
  move(const std::array<double, Constants::LOCATION_COORDS_COUNT> &newCoords);
  std::string toStr() const;
  bool coordsEqual(const std::vector<double> &otherCoords) const;
};
