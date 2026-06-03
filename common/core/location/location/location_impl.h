#pragma once

#include <algorithm>
#include <stdexcept>

#include "common/json/json_parser/json_parser.h"
#include "common/network/serializer/serializer.h"
#include "common/utils/str/str.h"
#include "common/logging/logger/logger.h"

namespace common {
template <typename T>
void Location<T>::logOperation(const std::string &operationName,
                               const common::coords_t<T> &coords) const {
  SPDLOG_LOGGER_DEBUG(common::Logger::instance().getInner(),
                      "common::Location {} called: coords={}", operationName,
                      common::toStr(coords.begin(), coords.end()));
}

template <typename T>
Location<T>::Location(const common::coords_t<T> &coords_) : coords(coords_) {}

template <typename T>
Location<T>::Location(const Location &other) : coords(other.coords) {
  logOperation("COPY constructor", coords);
}

template <typename T>
Location<T> &Location<T>::operator=(const Location &other) {
  logOperation("COPY operator", coords);
  if (&other != this) {
    this->coords = other.coords;
  }
  return *this;
}

template <typename T>
Location<T>::Location(Location &&other) noexcept
    : coords(std::move(other.coords)) {
  logOperation("MOVE constructor", coords);
}

template <typename T>
Location<T> &Location<T>::operator=(Location &&other) noexcept {
  logOperation("MOVE operator", coords);
  if (&other != this) {
    this->coords = std::move(other.coords);
  }
  return *this;
}

template <typename T>
template <typename Container>
  requires std::ranges::input_range<Container> &&
           std::ranges::sized_range<Container>
void Location<T>::move(const Container &newCoords) {
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

template <typename T> std::string Location<T>::toStr() const {
  int precision = 4;
  return common::toStr(coords.begin(), coords.end(), precision, '(', ')');
}

template <typename T>
bool Location<T>::coordsEqual(const std::vector<T> &otherCoords) const {
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

template <typename T> common::coords_t<T> Location<T>::getCoords() const {
  return coords;
}

template <typename T> size_t Location<T>::getCoordsCount() const {
  return coords.size();
}

template <typename T> nlohmann::json Location<T>::toJson() const {
  return nlohmann::json{{"loc", coords}};
}

template <typename T>
std::expected<Location<T>, std::string>
Location<T>::fromJsonStr(const std::string &str) {
  Location<T> loc{};
  auto locInfo =
      std::make_unique<JsonArrayInfo<float, constants::LOCATION_COORDS_COUNT>>(
          "loc",
          [&](const common::coords_t<> &coords) { loc.move(coords); },
          nlohmann::json::value_t::number_float);

  auto error =
      JsonParser<coords_t<>>::parseField(std::move(locInfo), str);
  if (error) {
    return std::unexpected(*error);
  }

  return loc;
}

template <typename T>
std::expected<binary_t, std::string> Location<T>::toBinary() const {
  auto serializeResult = Serializer::toBinary<decltype(coords)>(coords);
  if (!serializeResult) {
    return std::unexpected("Failed to serialize location");
  }
  return *serializeResult;
}

template <typename T>
std::expected<Location<T>, std::string>
Location<T>::fromBinary(const binary_t &binary) {
  coords_t coords{};

  bool deserialized = Serializer::fromBinary<decltype(coords)>(binary, coords);
  if (!deserialized) {
    return std::unexpected("Failed to serialize location");
  }

  return Location<T>{coords};
}
} // namespace common
