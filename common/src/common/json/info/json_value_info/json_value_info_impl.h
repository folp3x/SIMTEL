#pragma once

namespace common {
template <typename T>
template <typename U>
constexpr std::optional<nlohmann::json::value_t>
JsonValueInfo<T>::recognizeType() {
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
  } else {
    return std::nullopt;
  }
}

template <typename T>
JsonValueInfo<T>::JsonValueInfo(
    T *value_, const std::function<std::string(const T &)> &validateFunc_)
    : value(value_), validateFunc(validateFunc_) {}

template <typename T>
std::optional<std::string> JsonValueInfo<T>::parse(const nlohmann::json &json) {
  constexpr auto valueType = recognizeType<T>();

  if (valueType) {
    if (!utils::hasJsonType(json, *valueType)) {
      std::string typeStr = utils::jsonTypeToStr(*valueType);
      return "Expected value of type " + utils::quoted(typeStr);
    }

    T parsed = json.template get<T>();

    if (validateFunc) {
      std::string error = validateFunc(parsed);
      if (!error.empty()) {
        return error;
      }
    }

    *value = parsed;

    return std::nullopt;
  } else {
    throw std::invalid_argument("Unsupported JSON value type");
  }
}
} // namespace common
