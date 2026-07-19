#pragma once

namespace common {
template <typename T>
JsonVectorInfo<T>::JsonVectorInfo(
    std::vector<T> *value,
    const std::function<std::string(const T &)> &elemValidateFunc)
    : JsonContainerInfo<std::vector<T>>(value, elemValidateFunc) {}

template <typename T>
std::optional<std::string>
JsonVectorInfo<T>::parseContainer(const nlohmann::json &json) {
  constexpr auto elemType = this->template recognizeType<T>();
  static_assert(elemType.has_value(), "Unsupported type");

  for (size_t i = 0; i < json.size(); ++i) {
    nlohmann::json elemJson = json[i];
    if (!(utils::hasJsonType(elemJson, *elemType))) {
      std::string typeStr = utils::jsonTypeToStr(*elemType);
      return "Expected element of type " + utils::quoted(typeStr);
    }

    this->value->push_back(elemJson.get<T>());
  }

  return std::nullopt;
}
} // namespace common
