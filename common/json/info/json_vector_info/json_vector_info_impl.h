#pragma once

namespace common {
template <typename T>
JsonVectorInfo<T>::JsonVectorInfo(
    const std::string &name,
    const std::function<void(const std::vector<T> &)> &successCallback,
    const std::function<std::string(const std::vector<T> &)> &checkFn)
    : JsonContainerInfo<std::vector<T>>(name, successCallback, checkFn) {}

template <typename T>
std::expected<std::vector<T>, std::string>
JsonVectorInfo<T>::parseContainer(const nlohmann::json &fieldJson) {
  std::vector<T> field{};

  for (size_t i = 0; i < fieldJson.size(); ++i) {
    auto elemJson = fieldJson[i];
    auto elemType = this->template recognizeType<T>();
    if (!(hasJsonType(elemJson, elemType))) {
      return std::unexpected("elements must have a type: '" +
                             jsonTypeToStr(elemType) + "'");
    }
    T elem = elemJson.template get<T>();
    field.push_back(elem);
  }

  return field;
}
} // namespace common
