#pragma once

#include "common/core/request/request/request.h"
#include "common/types.h"

namespace common {
struct MeasurementReportRequest : Request {
  imei_t imei = "";
  imei_t imsi = "";
  unsigned int bsId = 0;

  MeasurementReportRequest(const imei_t &imei_, const imsi_t &imsi_,
                           unsigned int bsId_)
      : imei(imei_), imsi(imsi_), bsId(bsId_) {}

  std::string toStr() {
    return "{imei=" + imei + ", imsi=" + imsi +
           ", bsId=" + std::to_string(bsId) + "}";
  }
};
} // namespace common
