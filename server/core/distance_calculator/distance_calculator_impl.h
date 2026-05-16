#pragma once

namespace server {
template <typename Container>
  requires std::ranges::input_range<Container> &&
           std::ranges::sized_range<Container>
float DistanceCalculator::calc(const common::Location<> &loc,
                               const Container &coords) {

  if (coords.size() != loc.getCoordsCount()) {
    throw std::invalid_argument("coords size must be equal to loc coords size");
  }

  auto locCoords = loc.getCoords();
  float dist = 0;

  // вычисление евклидова расстояния
  auto it1 = locCoords.begin();
  auto it2 = coords.begin();
  while (it1 != locCoords.end() || it2 != coords.end()) {
    dist += pow(*it1 - *it2, 2);
    ++it1;
    ++it2;
  }

  return sqrt(dist);
}
} // namespace server
