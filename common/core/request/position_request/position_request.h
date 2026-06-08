#pragma once

#include <cstdint>

#include "common/core/location/location/location.h"
#include "common/types.h"

namespace common {
struct PositionRequest {
  imei_t imei = "";
  Location<> loc{};
};
} // namespace common
