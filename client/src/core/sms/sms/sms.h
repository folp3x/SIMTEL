#pragma once

#include <chrono>

#include "common/types.h"
#include "core/sms/sms_status/sms_status.h"

namespace client {
struct Sms {
  using unix_seconds_t = std::chrono::sys_time<std::chrono::seconds>;

  unsigned int id = 0;
  unix_seconds_t timeSent;
  unix_seconds_t timeReceived;
  common::msisdn_t sender = "";
  common::msisdn_t receiver = "";
  std::string content = "";
  SmsStatus status = SmsStatus::Pending;
};
} // namespace client
