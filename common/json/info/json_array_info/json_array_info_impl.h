#pragma once

namespace common {
template <typename T, size_t S>
JsonArrayInfo<T, S>::JsonArrayInfo(
    const std::string &name,
    const std::function<void(const std::array<T, S> &)> &successCallback,
    const std::function<std::string(const std::array<T, S> &)> &checkFn)
    : JsonContainerInfo<std::array<T, S>>(name, successCallback, checkFn) {}

template <typename T, size_t S>
std::expected<std::array<T, S>, std::string>
JsonArrayInfo<T, S>::parseContainer(const nlohmann::json &fieldJson) {
  std::array<T, S> field{};

  for (size_t i = 0; i < S; ++i) {
    nlohmann::json elemJson = fieldJson[i];
    nlohmann::json elemType = this->template recognizeType<T>();
    if (!(hasJsonType(elemJson, elemType))) {
      return std::unexpected("elements must have a type: '" +
                             jsonTypeToStr(elemType) + "'");
    }
    field[i] = elemJson.get<T>();
  }

  return field;
}
} // namespace common
