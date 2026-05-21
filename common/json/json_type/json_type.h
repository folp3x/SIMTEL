#pragma once

#include <nlohmann/json.hpp>

namespace common {
inline std::string jsonTypeToStr(nlohmann::json::value_t type) {
  switch (type) {
  case nlohmann::json::value_t::object:
    return "object";
  case nlohmann::json::value_t::array:
    return "array";
  case nlohmann::json::value_t::number_float:
    return "float";
  case nlohmann::json::value_t::number_integer:
    return "integer";
  case nlohmann::json::value_t::number_unsigned:
    return "unsigned";
  case nlohmann::json::value_t::boolean:
    return "boolean";
  case nlohmann::json::value_t::string:
    return "string";
  }

  return "unknown";
}

inline bool hasJsonType(const nlohmann::json &field,
                        nlohmann::json::value_t type) {
  switch (type) {
  case nlohmann::json::value_t::number_float:
    // любое число
    return field.is_number();
  case nlohmann::json::value_t::number_integer:
    // любое целое число
    return field.is_number_integer();
  case nlohmann::json::value_t::number_unsigned:
    return field.is_number_unsigned();
  }
  return field.type() == type;
}
} // namespace common
