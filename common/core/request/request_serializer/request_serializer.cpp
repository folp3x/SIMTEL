#include "request_serializer.h"

namespace common {
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

  return content;
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

    imei_t imei;
    auto imeiInfo = std::make_unique<JsonFieldInfo<imei_t>>(
        "imei", [&](const imei_t &imei_) { imei = imei_; },
        nlohmann::json::value_t::string);

    auto imeiParseError =
        JsonParser<imei_t>::parseField(std::move(imeiInfo), jsonStr);
    if (imeiParseError) {
      return std::unexpected(*imeiParseError);
    }

    auto loc = Location<>::fromJsonStr(jsonStr);
    if (!loc) {
      return std::unexpected(loc.error());
    }

    return RrcConnectionRequest{imei, *loc};
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
    nlohmann::json jsonObj;
    jsonObj["imei"] = req.imei;
    jsonObj["signal"] = req.signal;
    jsonObj["bsId"] = req.bsId;

    std::string jsonStr = jsonObj.dump();
    content = BinarySerializer::strToBinary(jsonStr);
    break;
  }
  }

  if (content.empty()) {
    return std::unexpected("Unsupported protocol");
  }

  return content;
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

    imei_t imei;
    unsigned int signal;
    unsigned int bsId;

    auto imeiInfo = std::make_unique<JsonFieldInfo<imei_t>>(
        "imei", [&](const imei_t &imei_) { imei = imei_; },
        nlohmann::json::value_t::string);

    auto imeiParseError =
        JsonParser<imei_t>::parseField(std::move(imeiInfo), jsonStr);
    if (imeiParseError) {
      return std::unexpected(*imeiParseError);
    }

    auto signalInfo = std::make_unique<JsonFieldInfo<unsigned int>>(
        "signal", [&](const unsigned int &signal_) { signal = signal_; },
        nlohmann::json::value_t::number_unsigned);

    auto signalParseError =
        JsonParser<unsigned int>::parseField(std::move(signalInfo), jsonStr);
    if (signalParseError) {
      return std::unexpected(*signalParseError);
    }

    auto bsIdInfo = std::make_unique<JsonFieldInfo<unsigned int>>(
        "bsId", [&](const unsigned int &bsId_) { bsId = bsId_; },
        nlohmann::json::value_t::number_unsigned);

    auto bsIdParseError =
        JsonParser<unsigned int>::parseField(std::move(bsIdInfo), jsonStr);
    if (bsIdParseError) {
      return std::unexpected(*bsIdParseError);
    }

    return MeasurementControlRequest{imei, signal, bsId};
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

    auto bsId = BinarySerializer::toBinary(req.bsId);
    if (!bsId) {
      return std::unexpected("BS id serialize error");
    }

    content.reserve(imei->size() + bsId->size());
    content.insert(content.end(), imei->begin(), imei->end());
    content.insert(content.end(), bsId->begin(), bsId->end());
    break;
  }
  case Protocol::JSON: {
    nlohmann::json jsonObj;
    jsonObj["imei"] = req.imei;
    jsonObj["bsId"] = req.bsId;

    std::string jsonStr = jsonObj.dump();
    content = BinarySerializer::strToBinary(jsonStr);
    break;
  }
  }

  if (content.empty()) {
    return std::unexpected("Unsupported protocol");
  }

  return content;
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
    auto imei = BinarySerializer::imeiFromBinary(imeiBinary);
    if (!imei) {
      return std::unexpected("IMEI deserialize error");
    }
    offset += IMEI_BINARY_BYTES;

    binary_t bsIdBinary(binary.begin() + offset, binary.end());
    auto bsId = BinarySerializer::fromBinary<unsigned int>(bsIdBinary);
    if (!bsId) {
      return std::unexpected("BS id deserialize error");
    }

    return MeasurementReportRequest{*imei, *bsId};
  }
  case Protocol::JSON: {
    std::string jsonStr = BinarySerializer::strFromBinary(binary);

    imei_t imei;
    unsigned int bsId = 0;

    auto imeiInfo = std::make_unique<JsonFieldInfo<imei_t>>(
        "imei", [&](const imei_t &imei_) { imei = imei_; },
        nlohmann::json::value_t::string);

    auto imeiParseError =
        JsonParser<imei_t>::parseField(std::move(imeiInfo), jsonStr);
    if (imeiParseError) {
      return std::unexpected(*imeiParseError);
    }

    auto bsIdInfo = std::make_unique<JsonFieldInfo<unsigned int>>(
        "bsId", [&](const unsigned int &bsId_) { bsId = bsId_; },
        nlohmann::json::value_t::number_unsigned);

    auto bsIdParseError =
        JsonParser<unsigned int>::parseField(std::move(bsIdInfo), jsonStr);
    if (bsIdParseError) {
      return std::unexpected(*bsIdParseError);
    }

    return MeasurementReportRequest{imei, bsId};
  }
  }

  return std::unexpected("Unsupported protocol");
}
} // namespace common
