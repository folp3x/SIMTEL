#pragma once

#include <cstdint>

namespace server {
enum class RequestType : uint8_t {
  SM_Transfer,
  SM_Submit,
  MO_Forward_SM,
  SM_Submit_Acknowledge,
  Forward_SM_Request,
  Forward_SM_Request_Acknowledge,
  SM_Delivery,
  SM_Delivery_Report
};
} // namespace server
