#pragma once

#include <condition_variable>
#include <functional>
#include <queue>

#include "client/core/ue/ue_state/ue_state.h"
#include "client/network/socket/socket.h"
#include "common/core/request/request/request.h"
#include "common/core/request/ussd_code_request/ussd_code_request.h"

namespace client {
class UeExchange {
private:
  using CallbackType = std::function<void(std::unique_ptr<common::Request>,
                                          const std::string &)>;

  struct RequestInfo {
    UeState state;
    std::unique_ptr<common::Request> req;
    CallbackType callback{};
  };

  static constexpr unsigned int RECEIVE_FROM_BS_SLEEP_MSEC = 1000;
  static constexpr unsigned int RECEIVE_FROM_BS_TIMEOUT_MSEC = 100;
  static constexpr unsigned int RECEIVE_SIGNAL_TIMEOUT_MSEC = 2000;
  static constexpr unsigned int RECEIVE_USSD_TIMEOUT_MSEC = 1000;

  bool running = true;

  common::Protocol curProtocol;

  unsigned int signalLevel = 0;

  std::mutex receiveMtx;
  Socket sock{};
  common::NetworkAddress serverAddr;

  bool connected = false;
  std::mutex requestsMtx;
  std::condition_variable requestsCv{};
  std::queue<RequestInfo> requests = {};

  template <std::derived_from<common::Request> T>
  std::expected<T, std::string> receiveResponse() const;

  template <std::derived_from<common::Request> T>
  std::expected<T, std::string>
  parseFromBytes(const common::binary_t &bytes) const;

  std::optional<std::string> sendRequest(std::unique_ptr<common::Request> req);

  std::expected<common::binary_t, std::string>
  receiveResponseData(common::RequestType &type) const;

  std::expected<std::unique_ptr<common::Request>, std::string>
  handleLocationUpdate(RequestInfo info);

public:
  explicit UeExchange(const common::NetworkAddress &serverAddr_);

  void sendRequests();

  void addRequest(const UeState &state, std::unique_ptr<common::Request> req,
                  const CallbackType &callback);

  std::optional<std::string> updateConnection(bool ueActive);

  void closeConnection();

  unsigned int getSignalLevel() const;
  bool hasSignal() const;

  void stop();

  void receiveFromBsInBackground(const CallbackType &callback);

  std::expected<std::unique_ptr<common::Request>, std::string>
  sendUssd(const UeState &state, std::unique_ptr<common::UssdCodeRequest> req);
};
} // namespace client

#include "ue_exchange_impl.h"
