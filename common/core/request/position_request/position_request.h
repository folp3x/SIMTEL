#pragma once

#include "common/core/location/location/location.h"
#include "common/core/request/request/request.h"
#include "common/types.h"

namespace common {
struct PositionRequest : Request {
  imei_t imei = "";
  Location<> loc{};

  PositionRequest(const imei_t &imei_, const Location<> &loc_)
      : imei(imei_), loc(loc_) {}
};
} // namespace common
