#include "request_serializer.h"

#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/network/network.h"

namespace common {
std::expected<SocketMessage, std::string>
RequestSerializer::requestMsgFromBytes(RequestType expectedType,
                                       const binary_t &binary,
                                       Protocol &protocol) {
  auto msg = socketMessageFromBinary(binary);
  if (!msg) {
    return msg;
  }
  auto parsedProtocol = protocolFromNetworkId(msg->header.protocol);
  if (!parsedProtocol) {
    return std::unexpected("Unknown protocol");
  }
  protocol = *parsedProtocol;
  auto reqType = static_cast<RequestType>(msg->header.reqType);
  if (reqType != expectedType) {
    return std::unexpected("Location_Update message expected");
  }

  return msg;
}

std::expected<binary_t, std::string>
RequestSerializer::requestToMsgBytes(Protocol protocol, RequestType reqType,
                                     const binary_t &content) {
  auto protocolId = protocolToNetworkId(protocol);
  if (!protocolId) {
    return std::unexpected("Unsupported protocol");
  }

  uint8_t requestTypeBinary = static_cast<uint8_t>(reqType);
  SocketMessage msg{
      {static_cast<uint32_t>(content.size()), *protocolId, requestTypeBinary},
      content};

  return socketMessagetoBinary(msg);
}

std::expected<binary_t, std::string>
RequestSerializer::measurementControlToBytes(
    Protocol protocol, const MeasurementControlRequest &req) {
  return req.toBytes(protocol);
}

std::expected<MeasurementControlRequest, std::string>
RequestSerializer::measurementControlFromBytes(const binary_t &bytes,
                                               Protocol &protocol) {
  MeasurementControlRequest req{};
  auto error = req.fromBytes(bytes, protocol);
  if (error) {
    return std::unexpected(*error);
  }
  return req;
}

std::expected<binary_t, std::string>
RequestSerializer::measurementReportToBytes(
    Protocol protocol, const MeasurementReportRequest &req) {
  return req.toBytes(protocol);
}

std::expected<MeasurementReportRequest, std::string>
RequestSerializer::measurementReportFromBytes(const binary_t &bytes,
                                              Protocol &protocol) {
  MeasurementReportRequest req{};
  auto error = req.fromBytes(bytes, protocol);
  if (error) {
    return std::unexpected(*error);
  }
  return req;
}

std::expected<binary_t, std::string>
RequestSerializer::rrcReconfigurationKeepToBytes(
    Protocol protocol, const RrcReconfigurationKeepRequest &req) {
  return req.toBytes(protocol);
}

std::expected<RrcReconfigurationKeepRequest, std::string>
RequestSerializer::rrcReconfigurationKeepFromBytes(const binary_t &bytes,
                                                   Protocol &protocol) {
  RrcReconfigurationKeepRequest req{};
  auto error = req.fromBytes(bytes, protocol);
  if (error) {
    return std::unexpected(*error);
  }
  return req;
}

std::expected<binary_t, std::string>
RequestSerializer::rrcReconfigurationHandoverToBytes(
    Protocol protocol, const RrcReconfigurationHandoverRequest &req) {
  return req.toBytes(protocol);
}

std::expected<RrcReconfigurationHandoverRequest, std::string>
RequestSerializer::rrcReconfigurationHandoverFromBytes(const binary_t &bytes,
                                                       Protocol &protocol) {
  RrcReconfigurationHandoverRequest req{};
  auto error = req.fromBytes(bytes, protocol);
  if (error) {
    return std::unexpected(*error);
  }
  return req;
}

std::expected<binary_t, std::string>
RequestSerializer::errorToBytes(Protocol protocol, const ErrorRequest &req) {
  return req.toBytes(protocol);
}

std::expected<ErrorRequest, std::string>
RequestSerializer::errorFromBytes(const binary_t &bytes, Protocol &protocol) {
  ErrorRequest req{};
  auto error = req.fromBytes(bytes, protocol);
  if (error) {
    return std::unexpected(*error);
  }
  return req;
}

std::expected<binary_t, std::string>
RequestSerializer::rrcReconfigurationCompleteToBytes(
    Protocol protocol, const RrcReconfigurationCompleteRequest &req) {
  return req.toBytes(protocol);
}

std::expected<RrcReconfigurationCompleteRequest, std::string>
RequestSerializer::rrcReconfigurationCompleteFromBytes(const binary_t &bytes,
                                                       Protocol &protocol) {
  RrcReconfigurationCompleteRequest req{};
  auto error = req.fromBytes(bytes, protocol);
  if (error) {
    return std::unexpected(*error);
  }
  return req;
}

std::expected<binary_t, std::string>
RequestSerializer::attachAcceptToBytes(Protocol protocol,
                                       const AttachAcceptRequest &req) {
  return req.toBytes(protocol);
}

std::expected<AttachAcceptRequest, std::string>
RequestSerializer::attachAcceptFromBytes(const binary_t &bytes,
                                         Protocol &protocol) {
  AttachAcceptRequest req{};
  auto error = req.fromBytes(bytes, protocol);
  if (error) {
    return std::unexpected(*error);
  }
  return req;
}
} // namespace common
