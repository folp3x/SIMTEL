#pragma once

namespace common {
template <typename T>
template <typename U>
nlohmann::json::value_t JsonFieldInfo<T>::recognizeType(bool arrayType) {
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
    const std::string &name,
    const std::function<void(const T &)> &successCallback_,
    const std::function<std::string(const T &)> &checkFn_, bool arrayType)
    : JsonBaseInfo(name), successCallback(successCallback_),
      type(recognizeType<T>(arrayType)), checkFn(checkFn_) {}

template <typename T>
std::optional<std::string> JsonFieldInfo<T>::parse(const nlohmann::json &json,
                                                   bool finalParse) {
  std::string showedName = name.empty() ? "root element" : getName(true);

  if (!name.empty() && !json.contains(name)) {
    return showedName + " is required";
  }

  nlohmann::json fieldJson = getFieldJson(json);
  if (!hasJsonType(fieldJson, type)) {
    return showedName + " must have a type '" + jsonTypeToStr(type) + "'";
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
} // namespace common
