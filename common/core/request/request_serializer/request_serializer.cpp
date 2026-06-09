#include "request_serializer.h"

namespace common {
std::expected<binary_t, std::string>
RequestSerializer::positionRequestToBinary(Protocol protocol,
                                           const PositionRequest &req) {
  auto protocolConvertResult = protocolToNetworkId(protocol);
  if (!protocolConvertResult) {
    return std::unexpected("Unsupported protocol");
  }
  uint8_t protocolId = *protocolConvertResult;

  binary_t content;
  switch (protocol) {
  case Protocol::BINARY: {
    auto locSerializeResult = req.loc.toBinary();
    if (!locSerializeResult) {
      return std::unexpected(locSerializeResult.error());
    }

    auto imeiSerializeResult = BinarySerializer::imeiToBinary(req.imei);
    if (!imeiSerializeResult) {
      return std::unexpected("IMEI serialize error");
    }

    content.reserve(imeiSerializeResult->size() + locSerializeResult->size());
    content.insert(content.end(), imeiSerializeResult->begin(),
                   imeiSerializeResult->end());
    content.insert(content.end(), locSerializeResult->begin(),
                   locSerializeResult->end());
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

std::expected<PositionRequest, std::string>
RequestSerializer::positionRequestFromBinary(uint8_t protocolId,
                                             const binary_t &binary) {
  auto protocolSearchResult = protocolFromNetworkId(protocolId);
  if (!protocolSearchResult) {
    throw std::unexpected("Unsupported protocol");
  }

  switch (*protocolSearchResult) {
  case Protocol::BINARY: {
    size_t offset = 0;
    binary_t imeiBinary(binary.begin() + offset,
                        binary.begin() + offset + constants::IMEI_BINARY_BYTES);

    auto imeiParseResult = BinarySerializer::imeiFromBinary(imeiBinary);
    if (!imeiParseResult) {
      return std::unexpected("IMEI deserialize error");
    }

    offset += constants::IMEI_BINARY_BYTES;

    auto locParseResult =
        Location<>::fromBinary(binary_t{binary.begin() + offset, binary.end()});
    if (!locParseResult) {
      return std::unexpected(locParseResult.error());
    }
    return PositionRequest{*imeiParseResult, *locParseResult};
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

    auto locParseResult = Location<>::fromJsonStr(jsonStr);
    if (!locParseResult) {
      return std::unexpected(locParseResult.error());
    }

    return PositionRequest{imei, *locParseResult};
  }
  }

  throw std::unexpected("Unsupported protocol");
}
} // namespace common
