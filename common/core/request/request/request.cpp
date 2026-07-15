#include "request.h"

#include "common/network/binary_serializer/binary_serializer.h"

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
    return std::unexpected(
        "Unexpected request type: " +
        common::requestTypeToStr(
            static_cast<common::RequestType>(msg->header.reqType)));
  }

  return msg;
}

std::expected<binary_t, std::string>
Request::reqToMsgBytes(Protocol protocol, const binary_t &content) const {
  auto protocolId = protocolToNetworkId(protocol);
  if (!protocolId) {
    return std::unexpected("Unsupported protocol");
  }

  uint8_t requestTypeBinary = static_cast<uint8_t>(getType());
  SocketMessage msg{
      {static_cast<uint32_t>(content.size()), *protocolId, requestTypeBinary},
      content};

  return BinarySerializer::socketMessagetoBinary(msg);
}

std::string Request::toStr() const {
  auto json = toJson();
  std::string contentStr =
      json.empty()
          ? ""
          : json.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace);
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
    nlohmann::json jsonObj = toJson();

    std::string jsonStr =
        jsonObj.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace);

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
