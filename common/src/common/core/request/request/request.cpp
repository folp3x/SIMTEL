#include "request.h"

#include "common/binary/binary_serializer/binary_serializer.h"

namespace common {
std::expected<SocketMessage, std::string>
Request::msgFromReqBytes(const binary_t &binary, Protocol &protocol) const {
  auto msg = BinarySerializer::socketMessageFromBinary(binary);
  if (!msg) {
    return std::unexpected(msg.error());
  }

  auto parsedProtocol = protocolFromNetworkId(msg->header.protocol);
  if (!parsedProtocol) {
    return std::unexpected("Unknown protocol");
  }

  protocol = *parsedProtocol;
  auto reqType = static_cast<RequestType>(msg->header.reqType);
  if (reqType != getType()) {
    std::string reqTypeStr = common::requestTypeToStr(
        static_cast<common::RequestType>(msg->header.reqType));
    return std::unexpected("Unexpected request type: " + reqTypeStr);
  }

  return msg;
}

std::expected<binary_t, std::string>
Request::reqToMsgBytes(Protocol protocol, const binary_t &content) const {
  auto protocolId = protocolToNetworkId(protocol);
  if (!protocolId) {
    return std::unexpected("Unsupported protocol");
  }

  uint8_t reqType = static_cast<uint8_t>(getType());
  auto msgSize = static_cast<uint32_t>(content.size());

  SocketMessageHeader header{msgSize, *protocolId, reqType};
  SocketMessage msg{header, content};

  return BinarySerializer::socketMessageToBinary(msg);
}

std::string Request::toJsonStr() const {
  auto json = toJson();
  return json.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace);
}

std::string Request::toStr() const {
  std::string contentStr = toJsonStr();
  return requestTypeToStr(getType()) + contentStr;
}

std::expected<binary_t, std::string> Request::toBytes(Protocol protocol) {
  binary_t content;
  switch (protocol) {
  case Protocol::Binary: {
    auto binary = toBinary();
    if (!binary) {
      return std::unexpected(binary.error());
    }

    content = std::move(*binary);
    break;
  }
  case Protocol::Json: {
    std::string jsonStr = toJsonStr();
    content = BinarySerializer::strToBinaryUnsized(jsonStr);
    break;
  }
  default:
    return std::unexpected("Unsupported protocol");
  }

  return reqToMsgBytes(protocol, content);
}

std::optional<std::string> Request::fromBytes(const binary_t &bytes,
                                              Protocol &protocol) {
  auto msg = msgFromReqBytes(bytes, protocol);
  if (!msg) {
    return msg.error();
  }

  switch (protocol) {
  case Protocol::Binary: {
    return fromBinary(msg->content);
  }
  case Protocol::Json: {
    std::string jsonStr = BinarySerializer::strFromBinaryUnsized(msg->content);
    return fromJsonStr(jsonStr);
  }
  default:
    return "Unsupported protocol";
  }
}
} // namespace common
