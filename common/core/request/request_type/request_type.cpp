#include "request_type.h"

#include <unordered_map>

#include "common/network/socket/socket_message/socket_message.h"

namespace common {
static std::unordered_map<RequestType, std::string> types = {
    {RequestType::Empty, "Empty"},
    {RequestType::Error, "Error"},
    {RequestType::Rrc_Connection, "Rrc_Connection"},
    {RequestType::Measurement_Control, "Measurement_Control"},
    {RequestType::Measurement_Report, "Measurement_Report"},
    {RequestType::Rrc_Reconfiguration_Keep, "Rrc_Reconfiguration_Keep"},
    {RequestType::Rrc_Reconfiguration_Handover, "Rrc_Reconfiguration_Handover"},
    {RequestType::Rrc_Reconfiguration_Complete, "Rrc_Reconfiguration_Complete"},
    {RequestType::Attach_Accept, "Attach_Accept"},
    {RequestType::SM_Transfer, "SM_Transfer"},
    {RequestType::SM_Delivery, "SM_Delivery"},
    {RequestType::SM_Delivery_Report, "SM_Delivery_Report"}};

std::expected<RequestType, std::string>
parseRequestType(const binary_t &bytes) {
  auto msg = socketMessageFromBinary(bytes);
  if (!msg) {
    return std::unexpected(msg.error());
  }

  return static_cast<RequestType>(msg->header.reqType);
}

std::string requestTypeToStr(RequestType type) {
  auto it = types.find(type);
  if (it == types.end()) {
    return "Unknown";
  }
  return it->second;
}
} // namespace common
