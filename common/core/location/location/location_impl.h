#pragma once

#include <algorithm>
#include <stdexcept>

#include "common/json/json_parser/json_parser.h"
#include "common/logging/logger/logger.h"
#include "common/network/binary_serializer/binary_serializer.h"
#include "common/utils/str/str.h"

namespace common {
template <typename T, size_t S>
void Location<T, S>::logOperation(const std::string &operationName,
                                  const common::coords_t<T, S> &coords) const {
  SPDLOG_LOGGER_DEBUG(common::Logger::instance().getInner(),
                      "common::Location {} called: coords={}", operationName,
                      common::toStr(coords.begin(), coords.end()));
}

template <typename T, size_t S>
Location<T, S>::Location(const common::coords_t<T, S> &coords_)
    : coords(coords_) {}

template <typename T, size_t S>
Location<T, S>::Location(const Location &other) : coords(other.coords) {
  logOperation("COPY constructor", coords);
}

template <typename T, size_t S>
Location<T, S> &Location<T, S>::operator=(const Location &other) {
  logOperation("COPY operator", coords);
  if (&other != this) {
    this->coords = other.coords;
  }
  return *this;
}

template <typename T, size_t S>
Location<T, S>::Location(Location &&other) noexcept
    : coords(std::move(other.coords)) {
  logOperation("MOVE constructor", coords);
}

template <typename T, size_t S>
Location<T, S> &Location<T, S>::operator=(Location &&other) noexcept {
  logOperation("MOVE operator", coords);
  if (&other != this) {
    this->coords = std::move(other.coords);
  }
  return *this;
}

template <typename T, size_t S>
template <typename Container>
  requires std::ranges::input_range<Container> &&
           std::ranges::sized_range<Container>
void Location<T, S>::move(const Container &newCoords) {
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

template <typename T, size_t S> std::string Location<T, S>::toStr() const {
  int precision = 4;
  return common::toStr(coords.begin(), coords.end(), precision, '(', ')');
}

template <typename T, size_t S>
bool Location<T, S>::coordsEqual(const std::vector<T> &otherCoords) const {
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

template <typename T, size_t S>
common::coords_t<T, S> Location<T, S>::getCoords() const {
  return coords;
}

template <typename T, size_t S> size_t Location<T, S>::getCoordsCount() const {
  return coords.size();
}

template <typename T, size_t S> nlohmann::json Location<T, S>::toJson() const {
  return nlohmann::json{{"loc", coords}};
}

template <typename T, size_t S>
std::expected<Location<T, S>, std::string>
Location<T, S>::fromJsonStr(const std::string &str) {
  Location<T, S> loc{};
  auto locInfo =
      std::make_unique<JsonArrayInfo<float, constants::LOCATION_COORDS_COUNT>>(
          "loc", [&](const common::coords_t<> &coords) { loc.move(coords); },
          nlohmann::json::value_t::number_float);

  auto error = JsonParser<coords_t<>>::parseField(std::move(locInfo), str);
  if (error) {
    return std::unexpected(*error);
  }

  return loc;
}

template <typename T, size_t S>
std::expected<binary_t, std::string> Location<T, S>::toBinary() const {
  auto serializeResult = BinarySerializer::toBinary<decltype(coords)>(coords);
  if (!serializeResult) {
    return std::unexpected("Failed to serialize location");
  }
  return *serializeResult;
}

template <typename T, size_t S>
std::expected<Location<T, S>, std::string>
Location<T, S>::fromBinary(const binary_t &binary) {
  coords_t coords{};

  bool deserialized =
      BinarySerializer::fromBinary<decltype(coords)>(binary, coords);
  if (!deserialized) {
    return std::unexpected("Failed to serialize location");
  }

  return Location<T, S>{coords};
}
} // namespace common
