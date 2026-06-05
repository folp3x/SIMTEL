#pragma once

namespace common {
template <typename T, size_t S>
void JsonArrayInfo<T, S>::logConstructor(
    const std::string &constructorType, const std::string &name,
    nlohmann::json::value_t elemType) const {
  SPDLOG_LOGGER_DEBUG(
      common::Logger::instance().getInner(),
      "common::JsonArrayInfo {} constructor called: name={}, type={}",
      constructorType, name, jsonTypeToStr(elemType));
}

template <typename T, size_t S>
JsonArrayInfo<T, S>::JsonArrayInfo(
    const std::string &name,
    const std::function<void(const std::array<T, S> &)> &successCallback,
    nlohmann::json::value_t elemType_,
    const std::function<std::string(const std::array<T, S> &)> &checkFn)
    : JsonFieldInfo<std::array<T, S>>(name, successCallback,
                                      nlohmann::json::value_t::array, checkFn),
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
std::optional<std::string>
JsonArrayInfo<T, S>::parse(const nlohmann::json &json, bool finalParse) {
  auto error = JsonFieldInfo<std::array<T, S>>::parse(json, false);
  if (error) {
    return error;
  }

  std::string nameQuoted = this->getName(true);

  const auto &fieldJson = this->getFieldJson(json);
  if (fieldJson.size() != S) {
    return nameQuoted + " must have exactly " + std::to_string(S) + " elements";
  }

  std::array<T, S> field{};

  for (size_t i = 0; i < S; ++i) {
    auto elemJson = fieldJson[i];
    if (!(hasJsonType(elemJson, elemType))) {
      return nameQuoted + " elements must have a type: '" +
             jsonTypeToStr(elemType) + "'";
    }
    field[i] = elemJson.template get<T>();
  }

  if (finalParse) {
    this->successCallback(field);
  }

  return std::nullopt;
}
} // namespace common
