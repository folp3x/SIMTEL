#pragma once

#include "common/network/socket/socket/socket.h"

namespace client {
class Socket : public common::Socket {
private:
  static constexpr unsigned int SEND_TIMEOUT_SEC = 10;

public:
  std::optional<std::string> connectTo(const common::NetworkAddress &address);

  bool setReceiveTimeout(unsigned int timeoutMsec);
  bool removeReceiveTimeout();
};
} // namespace client
