#pragma once

#include "client/network/socket/socket.h"
#include "common/network/network_address/network_address.h"

namespace client {
class UeExchange {
private:
  Socket sock{};
  common::NetworkAddress serverAddr;

  void logSendLocation(const std::string &dataStr) const;
  void logReceiveDistance(const std::string &dataStr) const;

public:
  explicit UeExchange(const common::NetworkAddress &serverAddr_);

  std::optional<std::string> updateConnection(bool ueActive);

  std::optional<std::string> sendLocation(common::Protocol protocol,
                                          const common::Location<> &loc) const;
  std::expected<float, std::string>
  receiveDistance(common::Protocol protocol) const;
};
} // namespace client
