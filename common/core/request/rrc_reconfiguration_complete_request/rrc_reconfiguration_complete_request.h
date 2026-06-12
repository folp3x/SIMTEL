#pragma once

#include "common/core/request/request/request.h"
#include "common/types.h"

namespace common {
struct RrcReconfigurationCompleteRequest : Request {
  imsi_t mTimsi = "";

  RrcReconfigurationCompleteRequest(const imsi_t &mTimsi_) : mTimsi(mTimsi_) {}

  std::string toStr() { return "{mTimsi=" + mTimsi + "}"; }
};
} // namespace common
