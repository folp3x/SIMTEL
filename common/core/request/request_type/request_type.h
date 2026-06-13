#pragma once

#include <cstdint>
#include <expected>

#include "common/network/protocol/protocol.h"
#include "common/types.h"

namespace common {
enum class RequestType : uint8_t {
  Unknown,
  Error,
  Rrc_Connection,
  Measurement_Control,
  Measurement_Report,
  Rrc_Reconfiguration_Keep,
  Rrc_Reconfiguration_Handover,
  Rrc_Reconfiguration_Complete,
  Attach_Accept,
  SM_Transfer,
  SM_Submit,
  MO_Forward_SM,
  SM_Submit_Acknowledge,
  Forward_SM_Request,
  Forward_SM_Request_Acknowledge,
  SM_Delivery,
  SM_Delivery_Report
};

std::expected<RequestType, std::string> parseRequestType(const binary_t &bytes);
} // namespace common
