#pragma once

#include <cstdint>
#include <optional>

namespace common {
enum class UssdCode : uint8_t {
  GetBalance = 105,
  GetPhoneNumber = 201,
  MaxValue
};

inline std::optional<UssdCode> ussdCodeFromNum(uint8_t num) {
  if (num < static_cast<uint8_t>(UssdCode::MaxValue)) {
    return static_cast<UssdCode>(num);
  }
  return std::nullopt;
}

inline uint8_t ussdCodeToNum(UssdCode code) {
  return static_cast<uint8_t>(code);
}
} // namespace common
