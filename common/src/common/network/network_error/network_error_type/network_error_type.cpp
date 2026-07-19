#include "network_error_type.h"

#include <unordered_map>

namespace common {
static const std::unordered_map<NetworkErrorType, std::string> types = {
    {NetworkErrorType::SendTimeout, "Send timeout"},
    {NetworkErrorType::ConnectionClosed, "Connection closed"},
    {NetworkErrorType::Other, "Other"},
    {NetworkErrorType::EmptyMessage, "Empty message"},
    {NetworkErrorType::LargeMessage, "Large message"},
    {NetworkErrorType::ReceiveTimeout, "Receive timeout"},
    {NetworkErrorType::BadFileDescriptor, "Not connected"},
    {NetworkErrorType::IncompleteHeader, "Incomplete header"},
    {NetworkErrorType::NoMsgSize, "Failed to read size"},
    {NetworkErrorType::IncompleteContent, "Incomplete content"}};

std::string networkErrorTypeToStr(NetworkErrorType type) {
  auto it = types.find(type);
  if (it == types.end()) {
    return "Unknown";
  }
  return it->second;
}
} // namespace common
