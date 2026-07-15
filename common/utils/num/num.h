#pragma once

#include <optional>

#include "common/core/ussd/ussd_code.h"

namespace common::utils {
inline std::optional<UssdCode> ussdCodeFromNum(uint8_t num) {
  auto maxValue = static_cast<uint8_t>(UssdCode::MaxValue);

  return (num < maxValue) ? std::optional{static_cast<UssdCode>(num)}
                          : std::nullopt;
}

inline uint8_t ussdCodeToNum(UssdCode code) {
  return static_cast<uint8_t>(code);
}
} // namespace common::utils
