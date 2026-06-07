#pragma once

#include "common/core/location/location/location.h"
#include "common/network/protocol/protocol.h"
#include "common/network/socket/socket/socket.h"

namespace client {
class Socket : public common::Socket {
private:
  static constexpr int RECEIVE_TIMEOUT_SEC = 10;

  static std::expected<int, std::string> initSock();

  void logSendLocation(const std::string &dataStr) const;
  void logReceiveDistance(const std::string &dataStr) const;

public:
  std::optional<std::string> connectTo(const common::NetworkAddress &address);

  std::optional<std::string> sendLocation(common::Protocol protocol,
                                          const common::Location<> &loc) const;
  std::expected<float, std::string>
  receiveDistance(common::Protocol protocol) const;
};
} // namespace client
