#pragma once

namespace common {
template <typename T>
std::unique_ptr<JsonValueInfo<T>> JsonParsable::makeJsonValue(
    T *value, const std::function<std::string(const T &)> &validateFunc) {
  return std::make_unique<JsonValueInfo<T>>(value, validateFunc);
}

template <typename T, size_t S>
  requires std::is_fundamental_v<T> || std::is_same_v<T, std::string>
std::unique_ptr<JsonArrayInfo<T, S>> JsonParsable::makeJsonArray(
    std::array<T, S> *value,
    const std::function<std::string(const T &)> &elemValidateFunc) {
  return std::make_unique<JsonArrayInfo<T, S>>(value, elemValidateFunc);
}

template <typename T>
  requires std::is_fundamental_v<T> || std::is_same_v<T, std::string>
std::unique_ptr<JsonVectorInfo<T>> JsonParsable::makeJsonVector(
    std::vector<T> *value,
    const std::function<std::string(const T &)> &elemValidateFunc) {
  return std::make_unique<JsonVectorInfo<T>>(value, elemValidateFunc);
}
} // namespace common
