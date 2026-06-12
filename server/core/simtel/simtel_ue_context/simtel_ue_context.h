#pragma once

#include "common/core/location/location/location.h"
#include "common/network/protocol/protocol.h"
#include "common/types.h"
#include "server/app/message_holder/message_holder.h"
#include "server/network/socket/socket.h"

namespace server {
class SimtelBaseStation;

class SimtelUeContext {
private:
  static constexpr int SOCK_RECEIVE_TIMEOUT_SEC = 4;

  bool mTimsiSet = false;
  common::imsi_t mTimsi;

  std::unique_ptr<Socket> sock;

  common::Protocol protocol = common::Protocol::JSON;

  SimtelBaseStation *bs = nullptr;

  std::mutex bufMtx;
  std::condition_variable bufCv{};
  common::binary_t buf = {};

public:
  SimtelUeContext(std::unique_ptr<Socket> sock_);

  common::imsi_t getMTimsi() const;
  bool setMTimsi(const common::imsi_t &mTimsi_);

  SimtelBaseStation *getBs() const;
  void setBs(SimtelBaseStation *bs_);

  common::Protocol getProtocol() const;
  void setProtocol(common::Protocol protocol_);

  common::binary_t takeBuf();
  void setBuf(const common::binary_t &buf_);

  std::optional<common::NetworkError> receiveData();
  std::optional<common::NetworkError> sendBufToUe();

  std::string toStr() const;

  bool notifyBs() const;

  bool setReceiveTimeout();
  bool removeReceiveTimeout();
};
} // namespace server
