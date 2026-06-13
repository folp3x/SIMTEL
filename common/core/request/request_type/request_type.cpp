#include "request_type.h"

#include "common/network/socket/socket_message/socket_message.h"

namespace common {
std::expected<RequestType, std::string>
parseRequestType(const binary_t &bytes) {
  auto msg = socketMessageFromBinary(bytes);
  if (!msg) {
    return std::unexpected(msg.error());
  }

  return static_cast<RequestType>(msg->header.reqType);
}
} // namespace common
