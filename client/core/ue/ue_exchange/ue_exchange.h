#pragma once

#include <functional>
#include <queue>

#include "client/core/ue/ue_context/ue_context.h"
#include "client/network/socket/socket.h"
#include "common/core/request/measurement_control_request/measurement_control_request.h"
#include "common/core/request/measurement_report_request/measurement_report_request.h"
#include "common/core/request/rrc_connection_request/rrc_connection_request.h"

namespace client {
class UeExchange {
private:
  using CallbackType = std::function<void(std::unique_ptr<common::Request>,
                                          const std::string &)>;

  struct RequestInfo {
    std::shared_ptr<const UeContext> ctx;
    common::RequestType type;
    CallbackType callback{};
  };

  static constexpr unsigned int UNKNOWN_BS_ID = 0;

  bool running = true;

  unsigned int curBsId = UNKNOWN_BS_ID;

  common::Protocol curProtocol;

  unsigned int signalLevel = 0;

  Socket sock{};
  common::NetworkAddress serverAddr;

  bool connected = false;
  std::mutex requestsMtx;
  std::condition_variable requestsCv{};
  std::queue<RequestInfo> requests = {};

  std::optional<std::string>
  sendLocationUpdate(const common::RrcConnectionRequest &req) const;

  std::expected<common::MeasurementControlRequest, std::string>
  receiveSignalLevel() const;

  std::optional<std::string>
  sendChosenBsId(const common::MeasurementReportRequest &req) const;

  std::expected<std::unique_ptr<common::Request>, std::string>
  receiveBsInfo() const;

public:
  explicit UeExchange(const common::NetworkAddress &serverAddr_);

  std::optional<std::string> handleLocationUpdate(const RequestInfo &info);

  void handleRequests();

  void addRequest(std::shared_ptr<const UeContext> ctx,
                  common::RequestType type, const CallbackType &callback);

  std::optional<std::string> updateConnection(bool ueActive);

  void closeConnection();

  unsigned int getSignalLevel() const;

  void stop();
};
} // namespace client
