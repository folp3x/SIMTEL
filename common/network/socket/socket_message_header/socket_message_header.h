#pragma once

#include <cstdint>
#include <expected>
#include <string>

#include "common/types.h"

namespace common {
struct SocketMessageHeader {
  uint32_t msgSize;
  uint8_t protocol;
  uint8_t msgType;
};

std::expected<SocketMessageHeader, std::string>
socketMessageHeaderFromBinary(const binary_t &binary);

std::expected<binary_t, std::string>
socketMessageHeaderToBinary(const SocketMessageHeader &header);
} // namespace common
