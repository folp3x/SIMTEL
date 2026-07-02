#pragma once

#include <cstdint>
#include <optional>

namespace common {
enum class UssdCode : uint8_t {
  GET_BALANCE = 105,
  GET_PHONE_NUMBER = 201,
  MAX_VALUE
};

inline std::optional<UssdCode> ussdCodeFromNum(uint8_t num) {
  if (num < static_cast<uint8_t>(UssdCode::MAX_VALUE)) {
    return static_cast<UssdCode>(num);
  }
  return std::nullopt;
}

inline uint8_t ussdCodeToNum(UssdCode code) {
  return static_cast<uint8_t>(code);
}
} // namespace common
