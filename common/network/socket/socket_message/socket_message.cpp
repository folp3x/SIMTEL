#include "socket_message.h"

namespace common {
std::expected<SocketMessage, std::string>
socketMessageFromBinary(const binary_t &binary) {
  SocketMessage message;

  auto headerResult = socketMessageHeaderFromBinary(binary);
  if (!headerResult) {
    return std::unexpected(headerResult.error());
  }
  message.header = *headerResult;

  if (binary.size() < sizeof(SocketMessageHeader) + message.header.msgSize) {
    return std::unexpected("Failed to deserialize content");
  }

  message.content.assign(binary.begin() + sizeof(SocketMessageHeader),
                         binary.begin() + sizeof(SocketMessageHeader) +
                             message.header.msgSize);

  return message;
}

std::expected<binary_t, std::string>
socketMessagetoBinary(const SocketMessage &msg) {
  auto headerResult = socketMessageHeaderToBinary(msg.header);
  if (!headerResult) {
    return std::unexpected(headerResult.error());
  }

  binary_t result = std::move(*headerResult);
  result.insert(result.end(), msg.content.begin(), msg.content.end());
  return result;
}
} // namespace common
