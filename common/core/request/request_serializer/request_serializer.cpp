#include "request_serializer.h"

#include "common/network/json_deserializer/json_deserializer.h"
#include "common/network/socket/socket_message/socket_message.h"

namespace common {
std::expected<SocketMessage, std::string>
RequestSerializer::requestMsgFromBytes(RequestType expectedType,
                                       const binary_t &binary) {
  auto msg = common::socketMessageFromBinary(binary);
  if (!msg) {
    return msg;
  }
  auto protocol = common::protocolFromNetworkId(msg->header.protocol);
  if (!protocol) {
    return std::unexpected("Unknown protocol");
  }
  auto reqType = static_cast<common::RequestType>(msg->header.msgType);
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

  uint8_t requestTypeBinary = static_cast<uint8_t>(RequestType::Rrc_Connection);
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

    content.reserve(imei->size() + loc->size());
    content.insert(content.end(), imei->begin(), imei->end());
    content.insert(content.end(), loc->begin(), loc->end());
    break;
  }
  case Protocol::JSON: {
    nlohmann::json jsonObj = req.loc.toJson();
    jsonObj["imei"] = req.imei;

    std::string jsonStr = jsonObj.dump();
    content = BinarySerializer::strToBinary(jsonStr);
    break;
  }
  }

  if (content.empty()) {
    return std::unexpected("Unsupported protocol");
  }

  return requestToMsgBytes(protocol, RequestType::Rrc_Connection, content);
}

std::expected<RrcConnectionRequest, std::string>
RequestSerializer::rrcConnectionFromBytes(uint8_t protocolId,
                                          const binary_t &binary) {
  auto protocol = protocolFromNetworkId(protocolId);
  if (!protocol) {
    throw std::unexpected("Unsupported protocol");
  }

  switch (*protocol) {
  case Protocol::BINARY: {
    size_t offset = 0;
    binary_t imeiBinary(binary.begin() + offset,
                        binary.begin() + offset + IMEI_BINARY_BYTES);
    auto imei = BinarySerializer::imeiFromBinary(imeiBinary);
    if (!imei) {
      return std::unexpected("IMEI deserialize error");
    }
    offset += IMEI_BINARY_BYTES;

    auto loc =
        Location<>::fromBinary(binary_t{binary.begin() + offset, binary.end()});
    if (!loc) {
      return std::unexpected(loc.error());
    }

    return RrcConnectionRequest{*imei, *loc};
  }
  case Protocol::JSON: {
    std::string jsonStr = BinarySerializer::strFromBinary(binary);

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
  }

  throw std::unexpected("Unsupported protocol");
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

    content.reserve(imei->size() + signal->size() + bsId->size());
    content.insert(content.end(), imei->begin(), imei->end());
    content.insert(content.end(), signal->begin(), signal->end());
    content.insert(content.end(), bsId->begin(), bsId->end());
    break;
  }
  case Protocol::JSON: {
    nlohmann::json jsonObj = nlohmann::json{
        {"imei", req.imei}, {"signal", req.signal}, {"bsId", req.bsId}};

    std::string jsonStr = jsonObj.dump();
    content = BinarySerializer::strToBinary(jsonStr);
    break;
  }
  }

  if (content.empty()) {
    return std::unexpected("Unsupported protocol");
  }

  return requestToMsgBytes(protocol, RequestType::Measurement_Control, content);
}

std::expected<MeasurementControlRequest, std::string>
RequestSerializer::measurementControlFromBytes(uint8_t protocolId,
                                               const binary_t &binary) {
  auto foundProtocol = protocolFromNetworkId(protocolId);
  if (!foundProtocol) {
    return std::unexpected("Unsupported protocol");
  }

  switch (*foundProtocol) {
  case Protocol::BINARY: {
    size_t offset = 0;

    if (binary.size() < offset + IMEI_BINARY_BYTES) {
      return std::unexpected("Binary too short for IMEI");
    }
    binary_t imeiBinary(binary.begin() + offset,
                        binary.begin() + offset + IMEI_BINARY_BYTES);
    auto imei = BinarySerializer::imeiFromBinary(imeiBinary);
    if (!imei) {
      return std::unexpected("IMEI deserialize error");
    }
    offset += IMEI_BINARY_BYTES;

    size_t signalSize = sizeof(decltype(MeasurementControlRequest::signal));
    if (binary.size() < offset + signalSize) {
      return std::unexpected("Binary too short for signal");
    }
    binary_t signalBinary(binary.begin() + offset,
                          binary.begin() + offset + signalSize);
    auto signal = BinarySerializer::fromBinary<unsigned int>(signalBinary);
    if (!signal) {
      return std::unexpected("Signal deserialize error");
    }
    offset += signalSize;

    size_t bsIdSize = sizeof(decltype(MeasurementControlRequest::bsId));
    if (binary.size() < offset + bsIdSize) {
      return std::unexpected("Binary too short for bsId");
    }
    binary_t bsIdBinary(binary.begin() + offset,
                        binary.begin() + offset + bsIdSize);
    auto bsId = BinarySerializer::fromBinary<unsigned int>(bsIdBinary);
    if (!bsId) {
      return std::unexpected("BS id deserialize error");
    }

    return MeasurementControlRequest{*imei, *signal, *bsId};
  }
  case Protocol::JSON: {
    std::string jsonStr = BinarySerializer::strFromBinary(binary);

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
  }

  return std::unexpected("Unsupported protocol");
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
      return std::unexpected("IMEI serialize error");
    }
    auto bsId = BinarySerializer::toBinary(req.bsId);
    if (!bsId) {
      return std::unexpected("BS id serialize error");
    }

    content.reserve(imsi->size() + bsId->size());
    content.insert(content.end(), imei->begin(), imei->end());
    content.insert(content.end(), imsi->begin(), imsi->end());
    content.insert(content.end(), bsId->begin(), bsId->end());
    break;
  }
  case Protocol::JSON: {
    nlohmann::json jsonObj = nlohmann::json{
        {"imei", req.imei}, {"imsi", req.imei}, {"bsId", req.bsId}};

    std::string jsonStr = jsonObj.dump();
    content = BinarySerializer::strToBinary(jsonStr);
    break;
  }
  }

  if (content.empty()) {
    return std::unexpected("Unsupported protocol");
  }

  return requestToMsgBytes(protocol, RequestType::Measurement_Report, content);
}

std::expected<MeasurementReportRequest, std::string>
RequestSerializer::measurementReportFromBytes(uint8_t protocolId,
                                              const binary_t &binary) {
  auto foundProtocol = protocolFromNetworkId(protocolId);
  if (!foundProtocol) {
    return std::unexpected("Unsupported protocol");
  }

  switch (*foundProtocol) {
  case Protocol::BINARY: {
    size_t offset = 0;

    if (binary.size() < offset + IMEI_BINARY_BYTES) {
      return std::unexpected("Binary too short for IMEI");
    }
    binary_t imeiBinary(binary.begin() + offset,
                        binary.begin() + offset + IMEI_BINARY_BYTES);
    auto imei = BinarySerializer::imsiFromBinary(imeiBinary);
    if (!imei) {
      return std::unexpected("IMEI deserialize error");
    }
    offset += IMEI_BINARY_BYTES;

    if (binary.size() < offset + IMSI_BINARY_BYTES) {
      return std::unexpected("Binary too short for IMSI");
    }
    binary_t imsiBinary(binary.begin() + offset,
                        binary.begin() + offset + IMSI_BINARY_BYTES);
    auto imsi = BinarySerializer::imsiFromBinary(imsiBinary);
    if (!imsi) {
      return std::unexpected("IMSI deserialize error");
    }
    offset += IMSI_BINARY_BYTES;

    binary_t bsIdBinary(binary.begin() + offset, binary.end());
    auto bsId = BinarySerializer::fromBinary<unsigned int>(bsIdBinary);
    if (!bsId) {
      return std::unexpected("BS id deserialize error");
    }

    return MeasurementReportRequest{*imei, *imsi, *bsId};
  }
  case Protocol::JSON: {
    std::string jsonStr = BinarySerializer::strFromBinary(binary);

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
  }

  return std::unexpected("Unsupported protocol");
}
} // namespace common
