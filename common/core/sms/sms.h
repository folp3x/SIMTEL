#pragma once

#include <chrono>

#include "common/types.h"

namespace common {
struct Sms {
  unsigned int id = 0;
  std::chrono::sys_time<std::chrono::seconds> timeSent;
  std::chrono::sys_time<std::chrono::seconds> timeReceived;
  common::msisdn_t sender = "";
  common::msisdn_t receiver = "";
  std::string content = "";
  bool delivered = false;
};
} // namespace common
