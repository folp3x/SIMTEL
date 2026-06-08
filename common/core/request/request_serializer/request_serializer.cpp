#include "request_serializer.h"

namespace common {
std::expected<binary_t, std::string>
RequestSerializer::positionRequestToBinary(const PositionRequest &req) {
  auto convertResult = protocolToNetworkId(req.protocol);
  if (!convertResult) {
    throw std::invalid_argument("Unsupported protocol");
  }
  uint8_t protocolId = *convertResult;

  switch (req.protocol) {
  case Protocol::BINARY: {
    auto serializeResult = req.loc.toBinary();
    if (!serializeResult) {
      return std::unexpected(serializeResult.error());
    }
    return *serializeResult;
  }
  case Protocol::JSON: {
    std::string jsonStr = req.loc.toJson().dump();
    binary_t binary = binary_t(jsonStr.begin(), jsonStr.end());

    return binary;
  }
  }

  throw std::invalid_argument("Unsupported protocol");
}

std::expected<PositionRequest, std::string>
RequestSerializer::positionRequestFromBinary(uint8_t protocol,
                                             const binary_t &binary) {
  auto protocolSearchResult = protocolFromNetworkId(protocol);
  if (!protocolSearchResult) {
    throw std::invalid_argument("Unsupported protocol");
  }

  Protocol foundProtocol = *protocolSearchResult;
  switch (foundProtocol) {
  case Protocol::BINARY: {
    auto parseResult = Location<>::fromBinary(binary);
    if (!parseResult) {
      return std::unexpected(parseResult.error());
    }
    return PositionRequest{foundProtocol, *parseResult};
  }
  case Protocol::JSON: {
    std::string jsonStr = std::string(binary.begin(), binary.end());
    auto parseResult = Location<>::fromJsonStr(jsonStr);
    if (!parseResult) {
      return std::unexpected(parseResult.error());
    }

    return PositionRequest{foundProtocol, *parseResult};
  }
  }

  throw std::invalid_argument("Unsupported protocol");
}
} // namespace common
