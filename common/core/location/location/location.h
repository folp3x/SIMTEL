#pragma once

#include <algorithm>
#include <ranges>

#include <expected>
#include <stdexcept>

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "common/json/json_parser/json_parser.h"
#include "common/network/binary_serializer/binary_serializer.h"
#include "common/types.h"
#include "common/utils/str/str.h"

namespace common {
template <typename T = float, size_t S = constants::LOCATION_COORDS_COUNT>
class Location {
private:
  coords_t<T, S> coords = {0};

  void logOperation(const std::string &operationName,
                    const coords_t<T, S> &coords) const;

public:
  Location() = default;
  explicit Location(const coords_t<T, S> &coords_);

  static std::expected<Location, std::string>
  fromJsonStr(const std::string &str);
  static std::expected<Location, std::string>
  fromBinary(const binary_t &binary);

  template <typename Container>
    requires std::ranges::input_range<Container> &&
             std::ranges::sized_range<Container>
  void move(const Container &newCoords);

  std::string toStr() const;

  bool coordsEqual(const std::vector<T> &otherCoords) const;

  coords_t<T, S> getCoords() const;
  size_t getCoordsCount() const;

  nlohmann::json toJson() const;
  std::expected<binary_t, std::string> toBinary() const;
};
} // namespace common

#include "location_impl.h"
