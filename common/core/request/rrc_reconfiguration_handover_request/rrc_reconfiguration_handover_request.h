#pragma once

#include "common/core/request/request/request.h"
#include "common/types.h"

namespace common {
struct RrcReconfigurationHandoverRequest : Request {
  imsi_t mTmsi = "";
  unsigned int ttl = 0;
  unsigned int bsId = 0;

  RrcReconfigurationHandoverRequest(const imsi_t &mTmsi_, unsigned int ttl_,
                                    unsigned int bsId_)
      : mTmsi(mTmsi_), ttl(ttl_), bsId(bsId_) {}
};
} // namespace common
