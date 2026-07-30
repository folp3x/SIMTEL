#pragma once

namespace common {
template <typename T, size_t S>
  requires std::is_arithmetic_v<T>
Location<T, S>::Location(const coords_t<T, S> &coords_) : coords(coords_) {}

template <typename T, size_t S>
  requires std::is_arithmetic_v<T>
template <typename Container>
  requires std::ranges::sized_range<Container>
void Location<T, S>::move(const Container &newCoords) {
  if (newCoords.size() > coords.size()) {
    throw std::invalid_argument("newCoords size must be <= " +
                                std::to_string(coords.size()));
  }
  if (std::ranges::empty(newCoords)) {
    throw std::invalid_argument("newCoords cant be empty" +
                                std::to_string(coords.size()));
  }
  std::ranges::copy(newCoords, coords.begin());
}

template <typename T, size_t S>
  requires std::is_arithmetic_v<T>
std::string Location<T, S>::toStr() const {
  return utils::toStr(coords.begin(), coords.end(), constants::RealNumPrecision,
                      '(', ')');
}

template <typename T, size_t S>
  requires std::is_arithmetic_v<T>
bool Location<T, S>::coordsEqual(const std::vector<T> &otherCoords) const {
  if (otherCoords.size() > coords.size()) {
    throw std::invalid_argument("otherCoords size must be <= " +
                                std::to_string(coords.size()));
  }
  if (otherCoords.empty()) {
    throw std::invalid_argument("otherCoords cant be empty" +
                                std::to_string(coords.size()));
  }
  return std::ranges::equal(otherCoords, coords);
}

template <typename T, size_t S>
  requires std::is_arithmetic_v<T>
coords_t<T, S> Location<T, S>::getCoords() const {
  return coords;
}

template <typename T, size_t S>
  requires std::is_arithmetic_v<T>
size_t Location<T, S>::getCoordsCount() const {
  return coords.size();
}
} // namespace common
