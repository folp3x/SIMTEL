#pragma once

#include <optional>

#include "common/core/request/position_request/position_request.h"
#include "common/network/protocol/protocol.h"
#include "common/types.h"

namespace common {
class RequestSerializer {
public:
  static std::expected<binary_t, std::string>
  positionRequestToBinary(Protocol protocol, const PositionRequest &req);

  static std::expected<PositionRequest, std::string>
  positionRequestFromBinary(uint8_t protocolId, const binary_t &binary);
};
} // namespace common
