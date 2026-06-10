#pragma once

#include "common/core/request/request/request.h"
#include "common/types.h"

namespace common {
struct MeasurementReportRequest : Request {
  imei_t imei = "";
  unsigned int bsId = 0;

  MeasurementReportRequest(const imei_t &imei_, unsigned int bsId_)
      : imei(imei_), bsId(bsId_) {}
};
} // namespace common
