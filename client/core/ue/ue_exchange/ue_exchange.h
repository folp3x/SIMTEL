#pragma once

#include <functional>
#include <queue>

#include "client/core/ue/ue_state/ue_state.h"
#include "client/network/socket/socket.h"
#include "common/core/request/attach_accept_request/attach_accept_request.h"
#include "common/core/request/measurement_control_request/measurement_control_request.h"
#include "common/core/request/measurement_report_request/measurement_report_request.h"
#include "common/core/request/rrc_connection_request/rrc_connection_request.h"
#include "common/core/request/rrc_reconfiguration_complete_request/rrc_reconfiguration_complete_request.h"

namespace client {
class UeExchange {
private:
  using CallbackType = std::function<void(std::unique_ptr<common::Request>,
                                          const std::string &)>;

  struct RequestInfo {
    UeState state;
    common::RequestType type = common::RequestType::Unknown;
    CallbackType callback{};
  };

  bool running = true;

  common::Protocol curProtocol;

  unsigned int signalLevel = 0;

  Socket sock{};
  common::NetworkAddress serverAddr;

  bool connected = false;
  std::mutex requestsMtx;
  std::condition_variable requestsCv{};
  std::queue<RequestInfo> requests = {};

  std::optional<std::string> sendRequest(std::unique_ptr<common::Request> req);

  std::expected<common::binary_t, std::string>
  receiveRequestData(common::RequestType &type) const;

  template <std::derived_from<common::Request> T>
  std::expected<T, std::string> receiveRequest() const;

  std::expected<common::MeasurementControlRequest, std::string>
  receiveSignalLevel() const;

  std::optional<std::string>
  sendChosenBsId(const common::MeasurementReportRequest &req) const;

  std::expected<std::unique_ptr<common::Request>, std::string>
  receiveBsInfo() const;

  std::expected<common::AttachAcceptRequest, std::string>
  receiveAttachAccept() const;

  std::optional<std::string>
  sendBsAccept(const common::RrcReconfigurationCompleteRequest &req) const;

public:
  explicit UeExchange(const common::NetworkAddress &serverAddr_);

  std::expected<std::unique_ptr<common::Request>, std::string>
  handleLocationUpdate(const RequestInfo &info);

  void handleRequests();

  void addRequest(const UeState &state, common::RequestType type,
                  const CallbackType &callback);

  std::optional<std::string> updateConnection(bool ueActive);

  void closeConnection();

  unsigned int getSignalLevel() const;
  bool hasSignal() const;

  void stop();
};
} // namespace client

#include "ue_exchange_impl.h"
