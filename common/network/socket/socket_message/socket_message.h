#pragma once

#include <vector>

#include "common/types.h"

namespace common {
struct SocketMessage {
  uint8_t protocol;
  binary_t content;
};
} // namespace common
