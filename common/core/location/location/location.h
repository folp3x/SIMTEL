#pragma once

#include <expected>
#include <nlohmann/json.hpp>
#include <ranges>
#include <string>
#include <vector>

#include "common/types.h"

namespace common {
template <typename T = float> class Location {
private:
  common::coords_t<T> coords = {0, 0, 0};

  void logOperation(const std::string &operationName,
                    const common::coords_t<T> &coords) const;

public:
  Location() = default;
  explicit Location(const common::coords_t<T> &coords_);
  Location(const Location &other);
  Location &operator=(const Location &other);
  Location(Location &&other) noexcept;
  Location &operator=(Location &&other) noexcept;

  template <typename Container>
    requires std::ranges::input_range<Container> &&
             std::ranges::sized_range<Container>
  void move(const Container &newCoords);

  std::string toStr() const;

  bool coordsEqual(const std::vector<T> &otherCoords) const;

  common::coords_t<T> getCoords() const;

  size_t getCoordsCount() const;

  nlohmann::json toJson() const;

  static std::expected<Location, std::string>
  fromJsonStr(const std::string &str);

  std::expected<binary_t, std::string> toBinary() const;

  static std::expected<Location, std::string>
  fromBinary(const binary_t &binary);
};
} // namespace common

#include "location_impl.h"
