#pragma once

namespace common {
template <typename T, size_t S>
Location<T, S>::Location(const coords_t<T, S> &coords_) : coords(coords_) {}

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
  constexpr unsigned int precision = 4;
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
coords_t<T, S> Location<T, S>::getCoords() const {
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
          "loc", [&](const coords_t<> &coords) { loc.move(coords); });

  auto error = JsonParser<coords_t<>>::parseField(std::move(locInfo), str);
  if (error) {
    return std::unexpected(*error);
  }

  return loc;
}

template <typename T, size_t S>
std::expected<binary_t, std::string> Location<T, S>::toBinary() const {
  auto binary = BinarySerializer::toBinary<>(coords);
  if (!binary) {
    return std::unexpected("Failed to serialize location");
  }
  return *binary;
}

template <typename T, size_t S>
std::expected<Location<T, S>, std::string>
Location<T, S>::fromBinary(const binary_t &binary) {
  coords_t coords{};

  bool deserialized =
      BinarySerializer::fromBinary<decltype(coords)>(binary, coords);
  if (!deserialized) {
    return std::unexpected("Failed to deserialize location");
  }

  return Location<T, S>{coords};
}
} // namespace common
