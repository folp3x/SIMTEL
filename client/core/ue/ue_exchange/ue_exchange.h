#pragma once

#include <functional>
#include <mutex>
#include <queue>

#include "client/core/ue/ue_context/ue_context.h"
#include "client/network/socket/socket.h"
#include "common/core/location/location/location.h"
#include "common/core/request/position_request/position_request.h"
#include "common/core/request/signal_request/signal_request.h"
#include "common/network/network_address/network_address.h"
#include "common/network/protocol/protocol.h"

namespace client {
class UeExchange {
private:
  using CallbackType = std::function<void(
      const std::optional<common::Request> &, const std::string &)>;

  struct RequestInfo {
    common::Protocol protocol;
    common::RequestType type;
    std::unique_ptr<common::Request> req{};
    CallbackType callback{};
  };

  mutable std::mutex signalLevelMtx;
  unsigned int signalLevel = 0;

  Socket sock{};
  common::NetworkAddress serverAddr;

  bool connected = false;
  std::mutex requestsMtx;
  std::condition_variable requestsCv{};
  std::queue<RequestInfo> requests = {};

  std::optional<std::string>
  sendLocationUpdate(common::Protocol protocol,
                     const common::PositionRequest &req) const;

  std::expected<common::SignalRequest, std::string> receiveSignalLevel() const;

public:
  explicit UeExchange(const common::NetworkAddress &serverAddr_);

  void handleRequests();

  void addRequest(common::Protocol protocol, common::RequestType type,
                  std::unique_ptr<common::Request> req,
                  const CallbackType &callback);

  std::optional<std::string> updateConnection(bool ueActive);

  void closeConnection();

  unsigned int getSignalLevel() const;
};
} // namespace client
