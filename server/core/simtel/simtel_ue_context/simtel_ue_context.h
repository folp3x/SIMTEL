#pragma once

#include "common/core/location/location/location.h"
#include "common/network/protocol/protocol.h"
#include "common/types.h"
#include "server/network/socket/socket.h"

namespace server {
class SimtelBaseStation;

class SimtelUeContext {
private:
  common::imsi_t imsi = "";

  std::unique_ptr<Socket> sock;

  common::Protocol protocol = common::Protocol::JSON;

  SimtelBaseStation *bs = nullptr;

  common::binary_t buf = {};

public:
  explicit SimtelUeContext(std::unique_ptr<Socket> sock_);

  common::imsi_t getImsi() const;
  void setImsi(const common::imsi_t &imsi_);

  SimtelBaseStation *getBs() const;
  void setBs(SimtelBaseStation *bs_);

  common::Protocol getProtocol() const;
  void setProtocol(common::Protocol protocol_);

  common::binary_t takeBuf();
  void setBuf(const common::binary_t &buf_);

  std::optional<std::string> receiveData();
  std::optional<std::string> sendBufToUe();
};
} // namespace server
