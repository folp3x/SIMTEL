#include "socket_message.h"

namespace common {
std::expected<SocketMessage, std::string>
socketMessageFromBinary(const binary_t &binary) {
  SocketMessage message;

  auto parsedHeader = socketMessageHeaderFromBinary(binary);
  if (!parsedHeader) {
    return std::unexpected(parsedHeader.error());
  }
  message.header = *parsedHeader;

  if (binary.size() <
      constants::SocketMessageHeaderBytes + message.header.msgSize) {
    return std::unexpected("Failed to deserialize content");
  }

  message.content.assign(binary.begin() + constants::SocketMessageHeaderBytes,
                         binary.end());

  return message;
}

std::expected<binary_t, std::string>
socketMessagetoBinary(const SocketMessage &msg) {
  auto parsedHeader = socketMessageHeaderToBinary(msg.header);
  if (!parsedHeader) {
    return std::unexpected(parsedHeader.error());
  }
  binary_t result = std::move(*parsedHeader);

  result.insert(result.end(), msg.content.begin(), msg.content.end());
  return result;
}
} // namespace common
