#pragma once

#include <expected>
#include <optional>

#include "common/core/request/request_type/request_type.h"
#include "common/network/socket/socket_message/socket_message.h"
#include "common/types.h"

namespace common {
class BinarySerializer {
public:
  static binary_t strToBinaryUnsized(const std::string &binary);
  static std::string strFromBinaryUnsized(const binary_t &binary);

  static std::expected<RequestType, std::string>
  parseRequestType(const binary_t &bytes);

  static std::expected<SocketMessage, std::string>
  socketMessageFromBinary(const binary_t &binary);

  static std::expected<binary_t, std::string>
  socketMessageToBinary(const SocketMessage &msg);

  static std::optional<SocketMessageHeader>
  socketMessageHeaderFromBinary(const binary_t &binary);

  static std::optional<binary_t>
  socketMessageHeaderToBinary(const SocketMessageHeader &header);
};
} // namespace common
