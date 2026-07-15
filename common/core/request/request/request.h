#pragma once

#include <nlohmann/json.hpp>

#include "common/core/request/request_type/request_type.h"

#include "common/binary/binary_serializer/binary_serializer.h"
#include "common/network/protocol/protocol.h"
#include "common/network/socket/socket_message/socket_message.h"

#include "common/binary/binary_serializable/binary_serializable.h"
#include "common/json/json_parsable/json_parsable.h"

namespace common {
class Request : public BinarySerializable, public JsonParsable {
private:
  std::expected<SocketMessage, std::string>
  msgFromReqBytes(const binary_t &binary, Protocol &protocol) const;
  std::expected<binary_t, std::string>
  reqToMsgBytes(Protocol protocol, const binary_t &content) const;

public:
  virtual ~Request() = default;

  virtual RequestType getType() const = 0;

  virtual std::string toStr() const;

  virtual nlohmann::json toJson() const = 0;

  virtual std::vector<std::unique_ptr<BaseBinaryInfo>>
  getBinaryValuesInfo() = 0;

  virtual std::unique_ptr<BaseJsonInfo> getJsonRootInfo() = 0;

  virtual std::expected<binary_t, std::string> toBytes(Protocol protocol);
  virtual std::optional<std::string> fromBytes(const binary_t &bytes,
                                               Protocol &protocol);
};
} // namespace common
