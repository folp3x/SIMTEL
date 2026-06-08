#pragma once

#include "common/core/location/location/location.h"
#include "common/network/protocol/protocol.h"
#include "common/types.h"
#include "server/core/simtel/simtel_base_station/simtel_base_station.h"
#include "server/network/socket/socket.h"

namespace server {
class SimtelUeContext {
private:
  const common::imsi_t UNKNOWN_IMSI = "";

  common::imsi_t imsi = UNKNOWN_IMSI;

  std::unique_ptr<Socket> sock;

  std::unique_ptr<SimtelBaseStation> bs = std::make_unique<SimtelBaseStation>();

  void logReceiveLocation(const std::string &dataStr) const;
  void logSendDistance(const std::string &dataStr) const;

public:
  explicit SimtelUeContext(std::unique_ptr<Socket> sock_);

  common::imsi_t getImsi() const;
  void setImsi(const common::imsi_t &imsi_);

  Socket *getSock() const;

  std::expected<common::Location<>, std::string>
  receiveLocation(common::Protocol &clientProtocol) const;
  std::optional<std::string> sendDistance(common::Protocol protocol,
                                          float distance) const;
};
} // namespace server
