#pragma once

#include "client/core/sms/sms_status/sms_status.h"

namespace client::utils {
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
} // namespace client::utils
