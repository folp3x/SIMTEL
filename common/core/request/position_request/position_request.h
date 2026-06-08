#pragma once

#include <cstdint>

#include "common/core/location/location/location.h"
#include "common/core/request/request/request.h"
#include "common/types.h"

namespace common {
struct PositionRequest : Request {
  common::Location<> loc{};
};
} // namespace common
