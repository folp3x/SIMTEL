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

public:
  explicit SimtelUeContext(std::unique_ptr<Socket> sock_);

  common::imsi_t getImsi() const;
  void setImsi(const common::imsi_t &imsi_);

  void setBs(SimtelBaseStation *bs_);

  Socket *getSock() const;

  void receiveLocationUpdate();

  void resendToUe(const common::binary_t &binary) const;
  void resendToBs(const common::binary_t &binary) const;
};
} // namespace server
