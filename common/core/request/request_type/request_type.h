#pragma once

#include <cstdint>

namespace common {
enum class RequestType : uint8_t {
  Unknown,
  Rrc_Connection,
  Measurement_Control,
  Measurement_Report,
  Rrc_Reconfiguration_Keep,
  Rrc_Reconfiguration_Handover,
  SM_Transfer,
  SM_Submit,
  MO_Forward_SM,
  SM_Submit_Acknowledge,
  Forward_SM_Request,
  Forward_SM_Request_Acknowledge,
  SM_Delivery,
  SM_Delivery_Report
};
} // namespace common
