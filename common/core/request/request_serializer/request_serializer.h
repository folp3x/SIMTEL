#pragma once

#include <optional>

#include "common/core/request/measurement_control_request/measurement_control_request.h"
#include "common/core/request/measurement_report_request/measurement_report_request.h"
#include "common/core/request/rrc_connection_request/rrc_connection_request.h"
#include "common/network/protocol/protocol.h"
#include "common/types.h"

namespace common {
class RequestSerializer {
private:
  static constexpr size_t IMEI_BINARY_BYTES = 8;
  static constexpr size_t IMSI_BINARY_BYTES = 8;

public:
  static std::expected<binary_t, std::string>
  rrcConnectionToBytes(Protocol protocol, const RrcConnectionRequest &req);

  static std::expected<RrcConnectionRequest, std::string>
  rrcConnectionFromBytes(uint8_t protocolId, const binary_t &binary);

  static std::expected<binary_t, std::string>
  measurementControlToBytes(Protocol protocol,
                            const MeasurementControlRequest &req);

  static std::expected<MeasurementControlRequest, std::string>
  measurementControlFromBytes(uint8_t protocolId, const binary_t &binary);

  static std::expected<binary_t, std::string>
  measurementReportToBytes(Protocol protocol,
                           const MeasurementReportRequest &req);

  static std::expected<MeasurementReportRequest, std::string>
  measurementReportFromBytes(uint8_t protocolId, const binary_t &binary);
};
} // namespace common
