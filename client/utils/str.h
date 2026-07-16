#pragma once

#include "client/core/sms/sms_status/sms_status.h"

namespace client::utils {
inline std::string smsStatusToStr(SmsStatus status) {
  switch (status) {
  case SmsStatus::Pending:
    return _("pending...");
  case SmsStatus::Delivered:
    return _("V (delivered)");
  case SmsStatus::NotDelivered:
    return _("X (not delivered)");
  default:
    return _("unknown");
  }
}
} // namespace client::utils
