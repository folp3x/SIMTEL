#pragma once

namespace common {
template <typename T, size_t S>
JsonArrayInfo<T, S>::JsonArrayInfo(
    std::array<T, S> *value,
    const std::function<std::string(const T &)> &elemValidateFunc)
    : JsonContainerInfo<std::array<T, S>>(value, elemValidateFunc) {}

template <typename T, size_t S>
std::optional<std::string>
JsonArrayInfo<T, S>::parseContainer(const nlohmann::json &json) {
  constexpr auto elemType = this->template recognizeType<T>();
  static_assert(elemType.has_value(), "Unsupported type");

  for (size_t i = 0; i < S; ++i) {
    nlohmann::json elemJson = json[i];
    if (!(utils::hasJsonType(elemJson, *elemType))) {
      std::string typeStr = utils::jsonTypeToStr(*elemType);
      return "Expected element of type " + utils::quoted(typeStr);
    }

    (*this->value)[i] = elemJson.get<T>();
  }

  return std::nullopt;
}
} // namespace common
