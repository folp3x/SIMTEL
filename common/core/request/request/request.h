#pragma once

#include <nlohmann/json.hpp>

#include "common/core/request/request_type/request_type.h"
#include "common/network/binary_serializer/binary_serializer.h"
#include "common/network/protocol/protocol.h"
#include "common/network/socket/socket_message/socket_message.h"

namespace common {
struct Request {
  virtual RequestType getType() const;

  virtual std::string toStr() const;

  virtual ~Request() = default;

  std::expected<SocketMessage, std::string>
  msgFromReqBytes(const binary_t &binary, Protocol &protocol) const;
  std::expected<binary_t, std::string>
  reqToMsgBytes(Protocol protocol, const binary_t &content) const;

  virtual nlohmann::json toJson() const = 0;
  virtual std::optional<std::string>
  fromJsonStr(const std::string &jsonStr) = 0;

  virtual std::expected<common::binary_t, std::string> toBinary() const = 0;
  virtual std::optional<std::string>
  fromBinary(const common::binary_t &binary) = 0;

  virtual std::expected<binary_t, std::string> toBytes(Protocol protocol) const;
  virtual std::optional<std::string> fromBytes(const binary_t &bytes,
                                               Protocol &protocol);
};
} // namespace common
