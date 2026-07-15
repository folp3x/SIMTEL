#pragma once

#include <expected>

#include "common/types.h"

namespace common {
enum class RequestType : uint8_t {
  Empty,
  Error,
  RrcConnection,
  MeasurementControl,
  MeasurementReport,
  RrcReconfigurationKeep,
  RrcReconfigurationHandover,
  RrcReconfigurationComplete,
  AttachAccept,
  SmTransfer,
  SmDelivery,
  SmDeliveryReport,
  SmDeliveryAck,
  SmDeliveryError,
  UssdCode,
  UssdBalance,
  UssdMsisdn
};

std::string requestTypeToStr(RequestType type);
} // namespace common
