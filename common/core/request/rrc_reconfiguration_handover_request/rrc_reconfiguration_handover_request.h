#pragma once

#include "common/core/request/request/request.h"
#include "common/types.h"

namespace common {
struct RrcReconfigurationHandoverRequest : Request {
  imsi_t mTimsi = "";
  unsigned int bsId = 0;

  RrcReconfigurationHandoverRequest(const imsi_t &mTimsi_, unsigned int bsId_)
      : mTimsi(mTimsi_), bsId(bsId_) {}
};
} // namespace common
