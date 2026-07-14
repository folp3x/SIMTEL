#pragma once

#include <expected>

#include "common/network/socket/socket_message_header/socket_message_header.h"

namespace common {
struct SocketMessage {
  SocketMessageHeader header{};
  binary_t content{};
};

std::expected<SocketMessage, std::string>
socketMessageFromBinary(const binary_t &binary);

std::expected<binary_t, std::string>
socketMessagetoBinary(const SocketMessage &msg);
} // namespace common
