#pragma once

#include "client/network/socket/socket.h"
#include "common/network/network_address/network_address.h"

namespace client {
class UeExchange {
private:
  Socket sock{};
  common::NetworkAddress serverAddr;

public:
  explicit UeExchange(const common::NetworkAddress &serverAddr_);

  std::optional<std::string> updateConnection(bool ueActive);
};
} // namespace client
