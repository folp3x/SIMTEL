#pragma once

#include "common/core/request/request/request.h"
#include "common/types.h"

namespace common {
struct MeasurementControlRequest : Request {
  imei_t imei = "";
  unsigned int signal = 0;
  unsigned int bsId = 0;

  MeasurementControlRequest(const imei_t &imei_, unsigned int signal_,
                            unsigned int bsId_)
      : imei(imei_), signal(signal_), bsId(bsId_) {}
};
} // namespace common
