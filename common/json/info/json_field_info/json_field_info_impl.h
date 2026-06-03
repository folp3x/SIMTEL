#pragma once

namespace common {
template <typename T>
void JsonFieldInfo<T>::logConstructor(const std::string &constructorType,
                                      const std::string &name,
                                      nlohmann::json::value_t type) const {
  SPDLOG_LOGGER_DEBUG(
      common::Logger::instance().getInner(),
      "common::JsonFieldInfo {} constructor called: name={}, type={}",
      constructorType, name, jsonTypeToStr(type));
}

template <typename T>
JsonFieldInfo<T>::JsonFieldInfo(
    const std::string &name_,
    const std::function<void(const T &)> &successCallback_,
    nlohmann::json::value_t type_,
    const std::function<std::string(const T &)> &checkFn_)
    : name(name_), successCallback(successCallback_), type(type_),
      checkFn(checkFn_) {}

template <typename T>
JsonFieldInfo<T>::JsonFieldInfo(const JsonFieldInfo &other)
    : name(other.name), successCallback(other.successCallback),
      type(other.type), checkFn(other.checkFn) {
  logConstructor("COPY", name, type);
}

template <typename T>
JsonFieldInfo<T>::JsonFieldInfo(JsonFieldInfo &&other) noexcept
    : name(std::move(other.name)),
      successCallback(std::move(other.successCallback)), type(other.type),
      checkFn(std::move(other.checkFn)) {
  logConstructor("MOVE", name, type);
}

template <typename T>
std::optional<std::string> JsonFieldInfo<T>::parse(const nlohmann::json &json) {
  std::string nameQuoted = getName(true);

  if (!json.contains(getName())) {
    return nameQuoted + " is required";
  }

  const auto &fieldJson = json[name];
  if (!hasJsonType(fieldJson, type)) {
    return nameQuoted + " must have a type '" + jsonTypeToStr(type) + "'";
  }

  T field = fieldJson.get<T>();

  if (checkFn) {
    std::string checkResult = checkFn(field);
    if (!checkResult.empty()) {
      return checkResult;
    }
  }

  successCallback(field);

  return std::nullopt;
}

template <typename T> std::string JsonFieldInfo<T>::getName(bool quoted) const {
  return quoted ? "'" + name + "'" : name;
}
} // namespace common
