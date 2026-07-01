#pragma once

#include <string>

namespace client {
enum class SmsStatus : uint8_t { PENDING, DELIVERED, NOT_DELIVERED };

inline std::string smsStatusToStr(SmsStatus status) {
  switch (status) {
  case SmsStatus::PENDING:
    return "pending";
  case SmsStatus::DELIVERED:
    return "delivered";
  case SmsStatus::NOT_DELIVERED:
    return "not delivered";
  default:
    return "unknown";
  }
}
} // namespace client
