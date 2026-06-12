#include "request_serializer.h"

#include "common/network/json_deserializer/json_deserializer.h"
#include "common/network/socket/socket_message/socket_message.h"
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
RequestSerializer::rrcConnectionToBytes(Protocol protocol,
                                        const RrcConnectionRequest &req) {
  binary_t content;
  switch (protocol) {
  case Protocol::BINARY: {
    auto loc = req.loc.toBinary();
    if (!loc) {
      return std::unexpected(loc.error());
    }

    auto imei = BinarySerializer::imeiToBinary(req.imei);
    if (!imei) {
      return std::unexpected("IMEI serialize error");
    }

    content = mergeBinary(*imei, *loc);
    break;
  }
  case Protocol::JSON: {
    nlohmann::json jsonObj = req.loc.toJson();
    jsonObj["imei"] = req.imei;

    std::string jsonStr = jsonObj.dump();
    content = BinarySerializer::strToBinary(jsonStr);
    break;
  }
  default:
    return std::unexpected("Unsupported protocol");
  }

  return requestToMsgBytes(protocol, RequestType::Rrc_Connection, content);
}

std::expected<RrcConnectionRequest, std::string>
RequestSerializer::rrcConnectionFromBytes(const binary_t &bytes,
                                          Protocol &protocol) {
  auto msg = requestMsgFromBytes(RequestType::Rrc_Connection, bytes, protocol);
  if (!msg) {
    return std::unexpected(msg.error());
  }

  switch (protocol) {
  case Protocol::BINARY: {
    auto curByte = msg->content.begin();

    if (curByte + IMEI_BINARY_BYTES > msg->content.end()) {
      return std::unexpected("Binary too short for IMEI");
    }
    binary_t imeiBinary(curByte, curByte + IMEI_BINARY_BYTES);
    auto imei = BinarySerializer::imeiFromBinary(imeiBinary);
    if (!imei) {
      return std::unexpected("IMEI deserialize error");
    }
    curByte += IMEI_BINARY_BYTES;

    auto loc = Location<>::fromBinary(binary_t{curByte, msg->content.end()});
    if (!loc) {
      return std::unexpected(loc.error());
    }

    return RrcConnectionRequest{*imei, *loc};
  }
  case Protocol::JSON: {
    std::string jsonStr = BinarySerializer::strFromBinary(msg->content);

    auto imei = JsonDeserializer::imeiFromJsonStr(jsonStr);
    if (!imei) {
      return std::unexpected(imei.error());
    }

    auto loc = Location<>::fromJsonStr(jsonStr);
    if (!loc) {
      return std::unexpected(loc.error());
    }

    return RrcConnectionRequest{*imei, *loc};
  }
  default:
    return std::unexpected("Unsupported protocol");
  }
}

std::expected<binary_t, std::string>
RequestSerializer::measurementControlToBytes(
    Protocol protocol, const MeasurementControlRequest &req) {
  binary_t content;
  switch (protocol) {
  case Protocol::BINARY: {
    auto imei = BinarySerializer::imeiToBinary(req.imei);
    if (!imei) {
      return std::unexpected("IMEI serialize error");
    }
    auto signal = BinarySerializer::toBinary(req.signal);
    if (!signal) {
      return std::unexpected("Signal serialize error");
    }
    auto bsId = BinarySerializer::toBinary(req.bsId);
    if (!bsId) {
      return std::unexpected("BS id serialize error");
    }

    content = mergeBinary(*imei, *signal, *bsId);
    break;
  }
  case Protocol::JSON: {
    nlohmann::json jsonObj = nlohmann::json{
        {"imei", req.imei}, {"signal", req.signal}, {"bsId", req.bsId}};

    std::string jsonStr = jsonObj.dump();
    content = BinarySerializer::strToBinary(jsonStr);
    break;
  }
  default:
    return std::unexpected("Unsupported protocol");
  }

  return requestToMsgBytes(protocol, RequestType::Measurement_Control, content);
}

std::expected<MeasurementControlRequest, std::string>
RequestSerializer::measurementControlFromBytes(const binary_t &bytes,
                                               Protocol &protocol) {
  auto msg =
      requestMsgFromBytes(RequestType::Measurement_Control, bytes, protocol);
  if (!msg) {
    return std::unexpected(msg.error());
  }

  switch (protocol) {
  case Protocol::BINARY: {
    auto curByte = msg->content.begin();

    if (curByte + IMEI_BINARY_BYTES > msg->content.end()) {
      return std::unexpected("Binary too short for IMEI");
    }
    binary_t imeiBinary(curByte, curByte + IMEI_BINARY_BYTES);
    auto imei = BinarySerializer::imeiFromBinary(imeiBinary);
    if (!imei) {
      return std::unexpected("IMEI deserialize error");
    }
    curByte += IMEI_BINARY_BYTES;

    size_t signalSize = sizeof(decltype(MeasurementControlRequest::signal));
    if (curByte + signalSize > msg->content.end()) {
      return std::unexpected("Binary too short for signal");
    }
    binary_t signalBinary(curByte, curByte + signalSize);
    auto signal = BinarySerializer::fromBinary<unsigned int>(signalBinary);
    if (!signal) {
      return std::unexpected("Signal deserialize error");
    }
    curByte += signalSize;

    size_t bsIdSize = sizeof(decltype(MeasurementControlRequest::bsId));
    if (curByte + bsIdSize > msg->content.end()) {
      return std::unexpected("Binary too short for bsId");
    }
    binary_t bsIdBinary(curByte, curByte + bsIdSize);
    auto bsId = BinarySerializer::fromBinary<unsigned int>(bsIdBinary);
    if (!bsId) {
      return std::unexpected("BS id deserialize error");
    }

    return MeasurementControlRequest{*imei, *signal, *bsId};
  }
  case Protocol::JSON: {
    std::string jsonStr = BinarySerializer::strFromBinary(msg->content);

    auto imei = JsonDeserializer::imeiFromJsonStr(jsonStr);
    if (!imei) {
      return std::unexpected(imei.error());
    }
    auto signal = JsonDeserializer::signalFromJsonStr(jsonStr);
    if (!signal) {
      return std::unexpected(signal.error());
    }
    auto bsId = JsonDeserializer::bsIdFromJsonStr(jsonStr);
    if (!bsId) {
      return std::unexpected(bsId.error());
    }

    return MeasurementControlRequest{*imei, *signal, *bsId};
  }
  default:
    return std::unexpected("Unsupported protocol");
  }
}

std::expected<binary_t, std::string>
RequestSerializer::measurementReportToBytes(
    Protocol protocol, const MeasurementReportRequest &req) {
  binary_t content;
  switch (protocol) {
  case Protocol::BINARY: {
    auto imei = BinarySerializer::imeiToBinary(req.imei);
    if (!imei) {
      return std::unexpected("IMEI serialize error");
    }
    auto imsi = BinarySerializer::imsiToBinary(req.imsi);
    if (!imsi) {
      return std::unexpected("IMSI serialize error");
    }
    auto bsId = BinarySerializer::toBinary(req.bsId);
    if (!bsId) {
      return std::unexpected("BS id serialize error");
    }

    content = mergeBinary(*imei, *imsi, *bsId);
    break;
  }
  case Protocol::JSON: {
    nlohmann::json jsonObj = nlohmann::json{
        {"imei", req.imei}, {"imsi", req.imsi}, {"bsId", req.bsId}};

    std::string jsonStr = jsonObj.dump();
    content = BinarySerializer::strToBinary(jsonStr);
    break;
  }
  default:
    return std::unexpected("Unsupported protocol");
  }

  return requestToMsgBytes(protocol, RequestType::Measurement_Report, content);
}

std::expected<MeasurementReportRequest, std::string>
RequestSerializer::measurementReportFromBytes(const binary_t &bytes,
                                              Protocol &protocol) {
  auto msg =
      requestMsgFromBytes(RequestType::Measurement_Report, bytes, protocol);
  if (!msg) {
    return std::unexpected(msg.error());
  }

  switch (protocol) {
  case Protocol::BINARY: {
    auto curByte = msg->content.begin();

    if (curByte + IMEI_BINARY_BYTES > msg->content.end()) {
      return std::unexpected("Binary too short for IMEI");
    }
    binary_t imeiBinary(curByte, curByte + IMEI_BINARY_BYTES);
    auto imei = BinarySerializer::imeiFromBinary(imeiBinary);
    if (!imei) {
      return std::unexpected("IMEI deserialize error");
    }
    curByte += IMEI_BINARY_BYTES;

    if (curByte + IMSI_BINARY_BYTES > msg->content.end()) {
      return std::unexpected("Binary too short for IMSI");
    }
    binary_t imsiBinary(curByte, curByte + IMSI_BINARY_BYTES);
    auto imsi = BinarySerializer::imsiFromBinary(imsiBinary);
    if (!imsi) {
      return std::unexpected("IMSI deserialize error");
    }
    curByte += IMSI_BINARY_BYTES;

    binary_t bsIdBinary(curByte, msg->content.end());
    auto bsId = BinarySerializer::fromBinary<unsigned int>(bsIdBinary);
    if (!bsId) {
      return std::unexpected("BS id deserialize error");
    }

    return MeasurementReportRequest{*imei, *imsi, *bsId};
  }
  case Protocol::JSON: {
    std::string jsonStr = BinarySerializer::strFromBinary(msg->content);

    auto imei = JsonDeserializer::imeiFromJsonStr(jsonStr);
    if (!imei) {
      return std::unexpected(imei.error());
    }
    auto imsi = JsonDeserializer::imsiFromJsonStr(jsonStr);
    if (!imsi) {
      return std::unexpected(imsi.error());
    }
    auto bsId = JsonDeserializer::bsIdFromJsonStr(jsonStr);
    if (!bsId) {
      return std::unexpected(bsId.error());
    }

    return MeasurementReportRequest{*imei, *imsi, *bsId};
  }
  default:
    return std::unexpected("Unsupported protocol");
  }
}

std::expected<binary_t, std::string>
RequestSerializer::rrcReconfigurationKeepToBytes(
    Protocol protocol, const RrcReconfigurationKeepRequest &req) {
  binary_t content;
  switch (protocol) {
  case Protocol::BINARY: {
    auto imei = BinarySerializer::imeiToBinary(req.imei);
    if (!imei) {
      return std::unexpected("IMEI serialize error");
    }
    auto bsId = BinarySerializer::toBinary(req.bsId);
    if (!bsId) {
      return std::unexpected("BS id serialize error");
    }

    content = mergeBinary(*imei, *bsId);
    break;
  }
  case Protocol::JSON: {
    nlohmann::json jsonObj =
        nlohmann::json{{"imei", req.imei}, {"bsId", req.bsId}};

    std::string jsonStr = jsonObj.dump();
    content = BinarySerializer::strToBinary(jsonStr);
    break;
  }
  default:
    return std::unexpected("Unsupported protocol");
  }

  return requestToMsgBytes(protocol, RequestType::Rrc_Reconfiguration_Keep,
                           content);
}

std::expected<RrcReconfigurationKeepRequest, std::string>
RequestSerializer::rrcReconfigurationKeepFromBytes(const binary_t &bytes,
                                                   Protocol &protocol) {
  auto msg = requestMsgFromBytes(RequestType::Rrc_Reconfiguration_Keep, bytes,
                                 protocol);
  if (!msg) {
    return std::unexpected(msg.error());
  }

  switch (protocol) {
  case Protocol::BINARY: {
    auto curByte = msg->content.begin();

    if (curByte + IMEI_BINARY_BYTES > msg->content.end()) {
      return std::unexpected("Binary too short for IMEI");
    }
    binary_t imeiBinary(curByte, curByte + IMEI_BINARY_BYTES);
    auto imei = BinarySerializer::imeiFromBinary(imeiBinary);
    if (!imei) {
      return std::unexpected("IMEI deserialize error");
    }
    curByte += IMEI_BINARY_BYTES;

    binary_t bsIdBinary(curByte, msg->content.end());
    auto bsId = BinarySerializer::fromBinary<unsigned int>(bsIdBinary);
    if (!bsId) {
      return std::unexpected("BS id deserialize error");
    }

    return RrcReconfigurationKeepRequest{*imei, *bsId};
  }
  case Protocol::JSON: {
    std::string jsonStr = BinarySerializer::strFromBinary(msg->content);

    auto imei = JsonDeserializer::imeiFromJsonStr(jsonStr);
    if (!imei) {
      return std::unexpected(imei.error());
    }
    auto bsId = JsonDeserializer::bsIdFromJsonStr(jsonStr);
    if (!bsId) {
      return std::unexpected(bsId.error());
    }

    return RrcReconfigurationKeepRequest{*imei, *bsId};
  }
  default:
    return std::unexpected("Unsupported protocol");
  }
}

std::expected<binary_t, std::string>
RequestSerializer::rrcReconfigurationHandoverToBytes(
    Protocol protocol, const RrcReconfigurationHandoverRequest &req) {
  binary_t content;
  switch (protocol) {
  case Protocol::BINARY: {
    auto mTimsi = BinarySerializer::imsiToBinary(req.mTimsi);
    if (!mTimsi) {
      return std::unexpected("IMSI serialize error");
    }
    auto bsId = BinarySerializer::toBinary(req.bsId);
    if (!bsId) {
      return std::unexpected("BS id serialize error");
    }

    content = mergeBinary(*mTimsi, *bsId);
    break;
  }
  case Protocol::JSON: {
    nlohmann::json jsonObj =
        nlohmann::json{{"mTimsi", req.mTimsi}, {"bsId", req.bsId}};

    std::string jsonStr = jsonObj.dump();
    content = BinarySerializer::strToBinary(jsonStr);
    break;
  }
  default:
    return std::unexpected("Unsupported protocol");
  }

  return requestToMsgBytes(protocol, RequestType::Rrc_Reconfiguration_Handover,
                           content);
}

std::expected<RrcReconfigurationHandoverRequest, std::string>
RequestSerializer::rrcReconfigurationHandoverFromBytes(const binary_t &bytes,
                                                       Protocol &protocol) {
  auto msg = requestMsgFromBytes(RequestType::Rrc_Reconfiguration_Handover,
                                 bytes, protocol);
  if (!msg) {
    return std::unexpected(msg.error());
  }

  switch (protocol) {
  case Protocol::BINARY: {
    auto curByte = msg->content.begin();

    if (curByte + IMSI_BINARY_BYTES > msg->content.end()) {
      return std::unexpected("Binary too short for m-TIMSI");
    }
    binary_t mTimsiBinary(curByte, curByte + IMSI_BINARY_BYTES);
    auto mTimsi = BinarySerializer::imsiFromBinary(mTimsiBinary);
    if (!mTimsi) {
      return std::unexpected("m-TIMSI deserialize error");
    }
    curByte += IMSI_BINARY_BYTES;

    binary_t bsIdBinary(curByte, msg->content.end());
    auto bsId = BinarySerializer::fromBinary<unsigned int>(bsIdBinary);
    if (!bsId) {
      return std::unexpected("BS id deserialize error");
    }

    return RrcReconfigurationHandoverRequest{*mTimsi, *bsId};
  }
  case Protocol::JSON: {
    std::string jsonStr = BinarySerializer::strFromBinary(msg->content);

    auto mTimsi = JsonDeserializer::imsiFromJsonStr(jsonStr, "mTimsi");
    if (!mTimsi) {
      return std::unexpected(mTimsi.error());
    }
    auto bsId = JsonDeserializer::bsIdFromJsonStr(jsonStr);
    if (!bsId) {
      return std::unexpected(bsId.error());
    }

    return RrcReconfigurationHandoverRequest{*mTimsi, *bsId};
  }
  default:
    return std::unexpected("Unsupported protocol");
  }
}

std::expected<binary_t, std::string>
RequestSerializer::errorToBytes(Protocol protocol, const ErrorRequest &req) {
  binary_t content;
  switch (protocol) {
  case Protocol::BINARY: {
    content = BinarySerializer::strToBinary(req.description);
    break;
  }
  case Protocol::JSON: {
    nlohmann::json jsonObj = nlohmann::json{{"description", req.description}};

    std::string jsonStr = jsonObj.dump();
    content = BinarySerializer::strToBinary(jsonStr);
    break;
  }
  default:
    return std::unexpected("Unsupported protocol");
  }

  return requestToMsgBytes(protocol, RequestType::Error, content);
}

std::expected<ErrorRequest, std::string>
RequestSerializer::errorFromBytes(const binary_t &bytes, Protocol &protocol) {
  auto msg = requestMsgFromBytes(RequestType::Error, bytes, protocol);
  if (!msg) {
    return std::unexpected(msg.error());
  }

  switch (protocol) {
  case Protocol::BINARY: {
    return ErrorRequest{BinarySerializer::strFromBinary(msg->content)};
  }
  case Protocol::JSON: {
    std::string jsonStr = BinarySerializer::strFromBinary(msg->content);

    auto description = JsonDeserializer::strFromJsonStr(jsonStr, "description");
    if (!description) {
      return std::unexpected(description.error());
    }

    return ErrorRequest{*description};
  }
  default:
    return std::unexpected("Unsupported protocol");
  }
}

std::expected<binary_t, std::string>
RequestSerializer::rrcReconfigurationCompleteToBytes(
    Protocol protocol, const RrcReconfigurationCompleteRequest &req) {
  binary_t content;
  switch (protocol) {
  case Protocol::BINARY: {
    auto mTimsi = BinarySerializer::imsiToBinary(req.mTimsi);
    if (!mTimsi) {
      return std::unexpected("m-TIMSI serialize error");
    }

    content = std::move(*mTimsi);
    break;
  }
  case Protocol::JSON: {
    nlohmann::json jsonObj = nlohmann::json{{"mTimsi", req.mTimsi}};

    std::string jsonStr = jsonObj.dump();
    content = BinarySerializer::strToBinary(jsonStr);
    break;
  }
  default:
    return std::unexpected("Unsupported protocol");
  }

  return requestToMsgBytes(protocol, RequestType::Rrc_Reconfiguration_Complete,
                           content);
}

std::expected<RrcReconfigurationCompleteRequest, std::string>
RequestSerializer::rrcReconfigurationCompleteFromBytes(const binary_t &bytes,
                                                       Protocol &protocol) {
  auto msg = requestMsgFromBytes(RequestType::Rrc_Reconfiguration_Complete,
                                 bytes, protocol);
  if (!msg) {
    return std::unexpected(msg.error());
  }

  switch (protocol) {
  case Protocol::BINARY: {
    auto curByte = msg->content.begin();

    binary_t mTimsiBinary(msg->content.begin(), msg->content.end());
    auto mTimsi = BinarySerializer::imsiFromBinary(mTimsiBinary);
    if (!mTimsi) {
      return std::unexpected("m-TIMSI deserialize error");
    }

    return RrcReconfigurationCompleteRequest{*mTimsi};
  }
  case Protocol::JSON: {
    std::string jsonStr = BinarySerializer::strFromBinary(msg->content);

    auto mTimsi = JsonDeserializer::imsiFromJsonStr(jsonStr, "mTimsi");
    if (!mTimsi) {
      return std::unexpected(mTimsi.error());
    }

    return RrcReconfigurationCompleteRequest{*mTimsi};
  }
  default:
    return std::unexpected("Unsupported protocol");
  }
}

std::expected<RequestType, std::string>
RequestSerializer::parseRequestType(const binary_t &bytes, Protocol &protocol) {
  auto msg = common::socketMessageFromBinary(bytes);
  if (!msg) {
    return std::unexpected(msg.error());
  }
  auto parsedProtocol = common::protocolFromNetworkId(msg->header.protocol);
  if (!parsedProtocol) {
    return std::unexpected("Invalid protocol");
  }
  protocol = *parsedProtocol;

  return static_cast<common::RequestType>(msg->header.reqType);
}
} // namespace common
