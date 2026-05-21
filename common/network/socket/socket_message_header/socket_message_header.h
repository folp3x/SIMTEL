#pragma once

#include <cstdint>

namespace common {
struct SocketMessageHeader {
  uint8_t protocol;
  uint32_t msgSize;
};
} // namespace common
