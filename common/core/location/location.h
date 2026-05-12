#pragma once

#include <algorithm>
#include <array>
#include <ranges>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>
#include <vector>

#include "common/constants/constants.h"
#include "common/types/types.h"
#include "common/utils/str/str.h"

namespace common {
template <typename T> class Location {
private:
  common::coords_t<T> coords = {0, 0, 0};

  void logOperation(const std::string &operationName,
                    const common::coords_t<T> &coords) const {
    SPDLOG_LOGGER_DEBUG(spdlog::default_logger(),
                        "common::Location {} called: coords={}", operationName,
                        common::toStr(coords.begin(), coords.end()));
  }

public:
  explicit Location(const common::coords_t<T> &coords_) : coords(coords_) {}

  Location(const Location &other) : coords(other.coords) {
    logOperation("COPY constructor", coords);
  }

  Location &operator=(const Location &other) {
    logOperation("COPY operator", coords);
    if (&other != this)
      this->coords = other.coords;
    return *this;
  }

  Location(Location &&other) noexcept : coords(std::move(other.coords)) {
    logOperation("MOVE constructor", coords);
  }

  Location &operator=(Location &&other) noexcept {
    logOperation("MOVE operator", coords);
    if (&other != this)
      this->coords = std::move(other.coords);
    return *this;
  }

  template <typename Container>
    requires std::ranges::input_range<Container> &&
             std::ranges::sized_range<Container>
  void move(const Container &newCoords) {
    if (newCoords.size() > coords.size()) {
      throw std::invalid_argument("newCoords size cant be bigger than " +
                                  std::to_string(coords.size()));
    }
    if (std::ranges::empty(newCoords)) {
      throw std::invalid_argument("newCoords cant be empty" +
                                  std::to_string(coords.size()));
    }
    std::ranges::copy(newCoords, coords.begin());
  }

  std::string toStr() const {
    constexpr int precision = 4;
    return common::toStr(coords.begin(), coords.end(), precision, '(', ')');
  }

  bool coordsEqual(const std::vector<T> &otherCoords) const {
    if (otherCoords.size() > coords.size()) {
      throw std::invalid_argument("otherCoords size cant be bigger than " +
                                  std::to_string(coords.size()));
    }
    if (otherCoords.empty()) {
      throw std::invalid_argument("newCoords cant be empty" +
                                  std::to_string(coords.size()));
    }
    return std::equal(otherCoords.begin(), otherCoords.end(), coords.begin());
  }

  common::coords_t<T> getCoords() const { return coords; }

  size_t getCoordsCount() const { return coords.size(); }
};
} // namespace common
