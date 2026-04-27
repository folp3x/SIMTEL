#include "location.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

// сдвигает текущие координаты в позицию newCoords
// если newCoords содержит не все координаты изменяется только часть координат
void Location::move(const std::vector<double> &newCoords) {
  if (newCoords.size() > COORDS_COUNT) {
    throw std::invalid_argument(
        "newCoords size cant be bigger than COORDS_COUNT=" +
        std::to_string(COORDS_COUNT));
  }

  std::copy(newCoords.begin(), newCoords.end(), coords.begin());
}

// сдвигает текущие координаты в позицию newCoords
void Location::move(const std::array<double, COORDS_COUNT> &newCoords) {
  std::copy(newCoords.begin(), newCoords.end(), coords.begin());
}

// возвращает текущие координаты в виде строки
std::string Location::toStr() const {
  std::ostringstream stream;
  stream << "(";
  for (int i = 0; i < COORDS_COUNT - 1; ++i) {
    // координаты округляются до 4 знаков
    stream << std::setprecision(4) << coords[i] << ", ";
  }
  stream << coords[COORDS_COUNT - 1] << ")";

  return stream.str();
}

// сравнивает текущие координаты с позицией otherCoords
bool Location::coordsEqual(const std::vector<double> &otherCoords) const {
  if (otherCoords.size() > COORDS_COUNT) {
    throw std::invalid_argument(
        "otherCoords size cant be bigger than COORDS_COUNT=" +
        std::to_string(COORDS_COUNT));
  }

  return std::equal(otherCoords.begin(), otherCoords.end(), coords.begin());
}
