#include "location.h"

#include <iomanip>
#include <sstream>

#include "common/types/types.h"
#include "common/utils/str/str.h"

namespace common {
Location::Location(const common::coords_t<float> &coords_) : coords(coords_) {};

std::string Location::toStr() const {
  constexpr int precision = 4;
  return common::toStr(coords.begin(), coords.end(), precision, '(', ')');
}

bool Location::coordsEqual(const std::vector<float> &otherCoords) const {
  if (otherCoords.size() > coords.size()) {
    throw std::invalid_argument(
        "otherCoords size cant be bigger than " +
        std::to_string(coords.size()));
  }

  if (otherCoords.empty()) {
    throw std::invalid_argument("newCoords cant be empty" +
                                std::to_string(coords.size()));
  }

  return std::equal(otherCoords.begin(), otherCoords.end(), coords.begin());
}

common::coords_t<float> Location::getCoords() const { return coords; }
} // namespace common
