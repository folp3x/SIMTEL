#pragma once

#include <cstdint>

namespace common {
enum class UssdCode : uint8_t {
  GetBalance = 105,
  GetPhoneNumber = 201,
  MaxValue
};
} // namespace common
