#pragma once

#include "common/core/request/request/request.h"
#include "common/types.h"

namespace common {
struct BsRequest : Request {
  imei_t imei = "";
  unsigned int bsId = 0;

  BsRequest(const imei_t &imei_, unsigned int bsId_)
      : imei(imei_), bsId(bsId_) {}
};
} // namespace common
