#pragma once

namespace common {
template <typename T, size_t S>
void JsonArrayInfo<T, S>::logConstructor(
    const std::string &constructorType, const std::string &name,
    nlohmann::json::value_t elemType) const {
  SPDLOG_LOGGER_DEBUG(
      Logger::instance().getInner(),
      "common::JsonArrayInfo {} constructor called: name={}, type={}",
      constructorType, name, jsonTypeToStr(elemType));
}

template <typename T, size_t S>
JsonArrayInfo<T, S>::JsonArrayInfo(
    const std::string &name,
    const std::function<void(const std::array<T, S> &)> &successCallback,
    nlohmann::json::value_t elemType_,
    const std::function<std::string(const std::array<T, S> &)> &checkFn)
    : JsonContainerInfo<std::array<T, S>>(name, successCallback, checkFn),
      elemType(elemType_) {}

template <typename T, size_t S>
JsonArrayInfo<T, S>::JsonArrayInfo(const JsonArrayInfo &other)
    : JsonFieldInfo<std::array<T, S>>(other.name, other.successCallback,
                                      other.type, other.checkFn),
      elemType(other.elemType) {
  logConstructor("COPY", this->name, elemType);
}

template <typename T, size_t S>
JsonArrayInfo<T, S>::JsonArrayInfo(JsonArrayInfo &&other) noexcept
    : JsonFieldInfo<std::array<T, S>>(std::move(other.name),
                                      std::move(other.successCallback),
                                      other.type, std::move(other.checkFn)),
      elemType(other.elemType) {
  logConstructor("MOVE", this->name, elemType);
}

template <typename T, size_t S>
std::expected<std::array<T, S>, std::string>
JsonArrayInfo<T, S>::parseContainer(const nlohmann::json &fieldJson) {
  std::array<T, S> field{};

  for (size_t i = 0; i < S; ++i) {
    auto elemJson = fieldJson[i];
    if (!(hasJsonType(elemJson, elemType))) {
      return std::unexpected("elements must have a type: '" +
                             jsonTypeToStr(elemType) + "'");
    }
    field[i] = elemJson.template get<T>();
  }

  return field;
}
} // namespace common
