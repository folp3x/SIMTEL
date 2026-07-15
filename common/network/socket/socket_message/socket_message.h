#pragma once

#include "common/network/socket/socket_message_header/socket_message_header.h"

namespace common {
struct SocketMessage {
  SocketMessageHeader header{};
  binary_t content{};
};
} // namespace common
