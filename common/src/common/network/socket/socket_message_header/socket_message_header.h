#pragma once

#include <cstdint>

namespace common {
struct SocketMessageHeader {
  uint32_t msgSize;
  uint8_t protocol;
  uint8_t reqType;
};
} // namespace common
