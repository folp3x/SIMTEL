#pragma once

#include <expected>

#include "common/network/socket/socket/socket.h"

namespace client {
class Socket : public common::Socket {
private:
  static constexpr int RECEIVE_TIMEOUT_SEC = 2;

  static std::expected<int, std::string> initSock();

public:
  std::optional<std::string> connectTo(const common::NetworkAddress &address);
};
} // namespace client
