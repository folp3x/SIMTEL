#pragma once

#include <concepts>

namespace common {
template <typename T>
void JsonFieldInfo<T>::logConstructor(const std::string &constructorType,
                                      const std::string &name) const {
  SPDLOG_LOGGER_DEBUG(Logger::instance().getInner(),
                      "common::JsonFieldInfo {} constructor called: name={}",
                      constructorType, name);
}

template <typename T>
template <typename U>
constexpr nlohmann::json::value_t
JsonFieldInfo<T>::recognizeType(bool arrayType) {
  if constexpr (std::is_same_v<U, std::string>) {
    return nlohmann::json::value_t::string;
  } else if constexpr (std::is_floating_point_v<U>) {
    return nlohmann::json::value_t::number_float;
  } else if constexpr (std::is_unsigned_v<U>) {
    return nlohmann::json::value_t::number_unsigned;
  } else if constexpr (std::is_signed_v<U>) {
    return nlohmann::json::value_t::number_integer;
  } else if constexpr (std::is_same_v<U, bool>) {
    return nlohmann::json::value_t::boolean;
  } else if (arrayType) {
    return nlohmann::json::value_t::array;
  } else {
    return nlohmann::json::value_t::object;
  }
}

template <typename T>
JsonFieldInfo<T>::JsonFieldInfo(
    const std::string &name_,
    const std::function<void(const T &)> &successCallback_,
    const std::function<std::string(const T &)> &checkFn_, bool arrayType)
    : name(name_), successCallback(successCallback_),
      type(recognizeType<T>(arrayType)), checkFn(checkFn_) {}

template <typename T>
JsonFieldInfo<T>::JsonFieldInfo(const JsonFieldInfo &other)
    : name(other.name), successCallback(other.successCallback),
      checkFn(other.checkFn) {
  logConstructor("COPY", name);
}

template <typename T>
JsonFieldInfo<T>::JsonFieldInfo(JsonFieldInfo &&other) noexcept
    : name(std::move(other.name)),
      successCallback(std::move(other.successCallback)),
      checkFn(std::move(other.checkFn)) {
  logConstructor("MOVE", name, type);
}

template <typename T>
std::optional<std::string> JsonFieldInfo<T>::parse(const nlohmann::json &json,
                                                   bool finalParse) {
  std::string nameQuoted = getName(true);

  if (!name.empty() && !json.contains(name)) {
    return nameQuoted + " is required";
  }

  const auto &fieldJson = getFieldJson(json);
  if (!hasJsonType(fieldJson, type)) {
    return nameQuoted + " must have a type '" + jsonTypeToStr(type) + "'";
  }

  T field = fieldJson.template get<T>();

  if (checkFn) {
    std::string checkResult = checkFn(field);
    if (!checkResult.empty()) {
      return checkResult;
    }
  }

  if (finalParse) {
    successCallback(field);
  }

  return std::nullopt;
}

template <typename T> std::string JsonFieldInfo<T>::getName(bool quoted) const {
  return quoted ? "'" + name + "'" : name;
}

template <typename T>
nlohmann::json
JsonFieldInfo<T>::getFieldJson(const nlohmann::json &json) const {
  return name.empty() ? json : json[name];
}
} // namespace common
