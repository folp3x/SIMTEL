#include "request_serializer.h"

namespace common {
std::expected<binary_t, std::string>
RequestSerializer::positionRequestToBinary(const PositionRequest &req) {
  auto convertResult = protocolToNetworkId(req.protocol);
  if (!convertResult) {
    throw std::invalid_argument("Unsupported protocol");
  }

  uint8_t protocolId = *convertResult;
  uint8_t requestType = static_cast<uint8_t>(req.type);

  auto imeiSerializeResult = BinarySerializer::imeiToBinary(req.imei);
  if (!imeiSerializeResult) {
    return std::unexpected("IMEI serialization error");
  }

  binary_t content;
  switch (req.protocol) {
  case Protocol::BINARY: {
    auto serializeResult = req.loc.toBinary();
    if (!serializeResult) {
      return std::unexpected(serializeResult.error());
    }
    content = std::move(*serializeResult);
    break;
  }
  case Protocol::JSON: {
    std::string jsonStr = req.loc.toJson().dump();
    content = binary_t(jsonStr.begin(), jsonStr.end());
    break;
  }
  }

  if (content.empty()) {
    throw std::invalid_argument("Unsupported protocol");
  }

  std::vector<uint8_t> result{};
  size_t binarySize = sizeof(protocolId) + sizeof(requestType) +
                      imeiSerializeResult->size() + content.size();

  result.reserve(binarySize);

  result.push_back(protocolId);
  result.push_back(requestType);
  result.insert(result.end(), imeiSerializeResult->begin(),
                imeiSerializeResult->end());
  result.insert(result.end(), content.begin(), content.end());

  return result;
}

std::expected<PositionRequest, std::string>
RequestSerializer::positionRequestFromBinary(const binary_t &binary) {
  PositionRequest req{};
  if (binary.size() < sizeof(req.protocol) + sizeof(req.type)) {
    return std::unexpected("Binary too short");
  }

  size_t offset = 0;
  uint8_t protocolId = binary[offset++];

  auto protocolSearchResult = protocolFromNetworkId(protocolId);
  if (!protocolSearchResult) {
    throw std::invalid_argument("Unsupported protocol");
  }

  req.protocol = *protocolSearchResult;
  uint8_t requestTypeNum = binary[offset++];
  req.type = static_cast<RequestType>(requestTypeNum);

  switch (req.protocol) {
  case Protocol::BINARY: {
    auto parseResult = Location<>::fromBinary(binary);
    if (!parseResult) {
      return std::unexpected(parseResult.error());
    }
    return req;
  }
  case Protocol::JSON: {
    std::string jsonStr = std::string(binary.begin(), binary.end());

    auto imeiInfo = std::make_unique<JsonFieldInfo<imei_t>>(
        "imei", [&](const imei_t &imei) { req.imei = imei; },
        nlohmann::json::value_t::string);

    auto imeiParseError =
        JsonParser<imei_t>::parseField(std::move(imeiInfo), jsonStr);
    if (imeiParseError) {
      return std::unexpected(*imeiParseError);
    }

    auto parseResult = Location<>::fromJsonStr(jsonStr);
    if (!parseResult) {
      return std::unexpected(parseResult.error());
    }

    req.loc = std::move(*parseResult);
  }
  }

  if (!protocolSearchResult) {
    throw std::invalid_argument("Unsupported protocol");
  }

  return req;
}
} // namespace common
