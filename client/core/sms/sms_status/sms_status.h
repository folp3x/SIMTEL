#pragma once

#include <string>

namespace client {
enum class SmsStatus : uint8_t { Pending, Delivered, NotDelivered };

inline std::string smsStatusToStr(SmsStatus status) {
  switch (status) {
  case SmsStatus::Pending:
    return "pending...";
  case SmsStatus::Delivered:
    return "V (delivered)";
  case SmsStatus::NotDelivered:
    return "X (not delivered)";
  default:
    return "unknown";
  }
}
} // namespace client
