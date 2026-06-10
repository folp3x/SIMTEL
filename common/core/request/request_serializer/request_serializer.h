#pragma once

#include <optional>

#include "common/core/request/measurement_control_request/measurement_control_request.h"
#include "common/core/request/measurement_report_request/measurement_report_request.h"
#include "common/core/request/rrc_connection_request/rrc_connection_request.h"
#include "common/network/protocol/protocol.h"
#include "common/network/socket/socket_message/socket_message.h"
#include "common/types.h"

namespace common {
class RequestSerializer {
private:
  static constexpr size_t IMEI_BINARY_BYTES = 8;
  static constexpr size_t IMSI_BINARY_BYTES = 8;

  static std::expected<binary_t, std::string>
  requestToMsgBytes(Protocol protocol, RequestType reqType,
                    const binary_t &content);

  static std::expected<SocketMessage, std::string>
  requestMsgFromBytes(RequestType expectedType, const binary_t &binary,
                      Protocol &protocol);

public:
  static std::expected<binary_t, std::string>
  rrcConnectionToBytes(Protocol protocol, const RrcConnectionRequest &req);

  static std::expected<RrcConnectionRequest, std::string>
  rrcConnectionFromBytes(const binary_t &bytes, Protocol &protocol);

  static std::expected<binary_t, std::string>
  measurementControlToBytes(Protocol protocol,
                            const MeasurementControlRequest &req);

  static std::expected<MeasurementControlRequest, std::string>
  measurementControlFromBytes(const binary_t &bytes);

  static std::expected<binary_t, std::string>
  measurementReportToBytes(Protocol protocol,
                           const MeasurementReportRequest &req);

  static std::expected<MeasurementReportRequest, std::string>
  measurementReportFromBytes(const binary_t &bytes);
};
} // namespace common
