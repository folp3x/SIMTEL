#pragma once

#include "common/core/request/request/request.h"
#include "common/types.h"

namespace common {
struct RrcReconfigurationKeepRequest : Request {
  imei_t imei = "";
  unsigned int bsId = 0;

  RrcReconfigurationKeepRequest(const imei_t &imei_, unsigned int bsId_)
      : imei(imei_), bsId(bsId_) {}

  std::string toStr() {
    return "{imei=" + imei + ", bsId=" + std::to_string(bsId) + "}";
  }
};
} // namespace common
