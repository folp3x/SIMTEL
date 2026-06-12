#pragma once

#include "common/core/location/location/location.h"
#include "common/core/request/request/request.h"
#include "common/types.h"

namespace common {
struct RrcConnectionRequest : Request {
  imei_t imei = "";
  Location<> loc{};

  RrcConnectionRequest(const imei_t &imei_, const Location<> &loc_)
      : imei(imei_), loc(loc_) {}

  std::string toStr() { return "{imei=" + imei + ", loc=" + loc.toStr() + "}"; }
};
} // namespace common
