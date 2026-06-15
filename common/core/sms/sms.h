#pragma once

#include <chrono>

#include "common/types.h"

namespace common {
struct Sms {
  std::chrono::sys_time<std::chrono::seconds> timeSent;
  std::chrono::sys_time<std::chrono::seconds> timeReceived;
  common::msisdn_t sender = "";
  common::msisdn_t receiver = "";
  std::string content = "";
  bool received = false;
};
} // namespace common
