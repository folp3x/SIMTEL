#include "location.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "common/types.h"
#include "common/utils/str.h"

namespace common {
Location::Location(const common::coords_t<float> &coords_) : coords(coords_) {};

// сдвигает текущие координаты в позицию newCoords
// если newCoords содержит не все координаты изменяется только часть координат
void Location::move(const std::vector<float> &newCoords) {
  if (newCoords.size() > coords.size()) {
    throw std::invalid_argument(
        "newCoords size cant be bigger than COORDS_COUNT=" +
        std::to_string(coords.size()));
  }

  if (newCoords.empty()) {
    throw std::invalid_argument("newCoords cant be empty" +
                                std::to_string(coords.size()));
  }

  std::copy(newCoords.begin(), newCoords.end(), coords.begin());
}

// сдвигает текущие координаты в позицию newCoords
void Location::move(const common::coords_t<float> &newCoords) {
  std::copy(newCoords.begin(), newCoords.end(), coords.begin());
}

// возвращает текущие координаты в виде строки
std::string Location::toStr() const {
  constexpr int precision = 4;
  return common::toStr(coords.begin(), coords.end(), precision, '(', ')');
}

// сравнивает текущие координаты с позицией otherCoords
bool Location::coordsEqual(const std::vector<float> &otherCoords) const {
  if (otherCoords.size() > coords.size()) {
    throw std::invalid_argument(
        "otherCoords size cant be bigger than COORDS_COUNT=" +
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
