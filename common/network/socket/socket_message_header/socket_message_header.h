#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "common/types.h"

namespace common {
struct SocketMessageHeader {
  uint32_t msgSize;
  uint8_t protocol;
  uint8_t reqType;
};

std::optional<SocketMessageHeader>
socketMessageHeaderFromBinary(const binary_t &binary);

std::optional<binary_t>
socketMessageHeaderToBinary(const SocketMessageHeader &header);
} // namespace common
