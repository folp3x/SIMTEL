#pragma once

#include "common/core/request/request/request.h"

namespace common {
struct PositionRequest : Request {
  unsigned int signal = 0;

  PositionRequest(unsigned int signal_) : signal(signal_) {}
};
} // namespace common
