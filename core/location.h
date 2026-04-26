#ifndef LOCATION_H
#define LOCATION_H

#include <array>
#include <vector>
#include <string>

class Location {
private:
  static const int COORDS_COUNT = 3;
  std::array<double, COORDS_COUNT> coords = {0, 0, 0};

public:
  void move(const std::vector<double>& newCoords);
  std::string toStr() const;
  bool coordsEqual(const std::vector<double> &otherCoords) const;
};

#endif // LOCATION_H
