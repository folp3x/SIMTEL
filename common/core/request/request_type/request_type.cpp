#include "request_type.h"

#include <unordered_map>

#include "common/network/socket/socket_message/socket_message.h"

namespace common {
static std::unordered_map<RequestType, std::string> types = {
    {RequestType::Empty, "Empty"},
    {RequestType::Error, "Error"},
    {RequestType::RrcConnection, "Rrc_Connection"},
    {RequestType::MeasurementControl, "Measurement_Control"},
    {RequestType::MeasurementReport, "Measurement_Report"},
    {RequestType::RrcReconfigurationKeep, "Rrc_Reconfiguration_Keep"},
    {RequestType::RrcReconfigurationHandover, "Rrc_Reconfiguration_Handover"},
    {RequestType::RrcReconfigurationComplete, "Rrc_Reconfiguration_Complete"},
    {RequestType::AttachAccept, "Attach_Accept"},
    {RequestType::SmTransfer, "SM_Transfer"},
    {RequestType::SmDelivery, "SM_Delivery"},
    {RequestType::SmDeliveryReport, "SM_Delivery_Report"},
    {RequestType::SmDeliveryAck, "SM_Delivery_Ack"},
    {RequestType::SmDeliveryError, "SM_Delivery_Error"},
    {RequestType::UssdCode, "Ussd_Code"},
    {RequestType::UssdBalance, "Ussd_Balance"},
    {RequestType::UssdMsisdn, "Ussd_Msisdn"}};

std::string requestTypeToStr(RequestType type) {
  auto it = types.find(type);
  if (it == types.end()) {
    return "Unknown";
  }
  return it->second;
}
} // namespace common
