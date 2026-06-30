#pragma once

#include <chrono>

#include "client/core/sms/sms_status/sms_status.h"
#include "common/types.h"

namespace client {
struct Sms {
  unsigned int id = 0;
  std::chrono::sys_time<std::chrono::seconds> timeSent;
  std::chrono::sys_time<std::chrono::seconds> timeReceived;
  common::msisdn_t sender = "";
  common::msisdn_t receiver = "";
  std::string content = "";
  SmsStatus status = SmsStatus::PENDING;
};
} // namespace client
