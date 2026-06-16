#pragma once

#include <expected>

#include "common/types.h"

namespace common {
enum class RequestType : uint8_t {
  Empty,
  Error,
  Rrc_Connection,
  Measurement_Control,
  Measurement_Report,
  Rrc_Reconfiguration_Keep,
  Rrc_Reconfiguration_Handover,
  Rrc_Reconfiguration_Complete,
  Attach_Accept,
  SM_Transfer,
  SM_Delivery,
  SM_Delivery_Report
};

std::expected<RequestType, std::string> parseRequestType(const binary_t &bytes);
std::string requestTypeToStr(RequestType type);
} // namespace common
