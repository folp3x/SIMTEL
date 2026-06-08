#pragma once

#include "client/network/socket/socket.h"
#include "common/core/location/location/location.h"
#include "common/network/network_address/network_address.h"
#include "common/network/protocol/protocol.h"

namespace client {
class UeExchange {
private:
  Socket sock{};
  common::NetworkAddress serverAddr;

public:
  explicit UeExchange(const common::NetworkAddress &serverAddr_);

  std::optional<std::string> updateConnection(bool ueActive);

  std::optional<std::string>
  sendLocationUpdate(common::Protocol protocol, const common::imei_t &imei,
                     const common::Location<> &loc) const;

  void closeConnection();
};
} // namespace client
