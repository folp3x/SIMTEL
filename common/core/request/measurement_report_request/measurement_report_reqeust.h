#pragma once

#include "common/core/request/request/request.h"
#include "common/types.h"

namespace common {
struct MeasurementReportRequest : Request {
  imei_t imsi = "";
  unsigned int bsId = 0;

  MeasurementReportRequest(const imsi_t &imsi_, unsigned int bsId_)
      : imsi(imsi_), bsId(bsId_) {}
};
} // namespace common
