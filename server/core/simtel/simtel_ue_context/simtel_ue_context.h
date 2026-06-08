#pragma once

#include "common/core/location/location/location.h"
#include "common/network/protocol/protocol.h"
#include "common/types.h"
#include "server/network/socket/socket.h"

namespace server {
class SimtelBaseStation;

class SimtelUeContext {
private:
  const common::imsi_t UNKNOWN_IMSI = "";

  common::imsi_t imsi = UNKNOWN_IMSI;

  std::unique_ptr<Socket> sock;

  common::Protocol protocol = common::Protocol::JSON;

  SimtelBaseStation *bs = nullptr;

  void logReceiveLocation(const std::string &dataStr) const;
  void logSendDistance(const std::string &dataStr) const;

public:
  explicit SimtelUeContext(std::unique_ptr<Socket> sock_);

  common::imsi_t getImsi() const;
  void setImsi(const common::imsi_t &imsi_);

  Socket *getSock() const;

  void setProtocol(common::Protocol protocol_);

  std::expected<common::Location<>, std::string>
  receiveLocation(common::Protocol &clientProtocol) const;
  std::optional<std::string> sendDistance(common::Protocol protocol,
                                          float distance) const;

  void resendToUe() const;
  void resendToBs() const;
};
} // namespace server
