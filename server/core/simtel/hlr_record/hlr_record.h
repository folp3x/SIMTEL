#pragma once

#include "common/types.h"

namespace server {
struct HlrRecord {
  int id;
  common::imei_t imei;
  common::imsi_t imsi;
  common::msisdn_t msisdn;
  std::string status;
  unsigned int mmeId = 0;

  std::string toStr() const {
    return "[id=" + std::to_string(id) + ", imsi=" + imsi + ", imei=" + imei +
           ", msisdn=" + msisdn + ", status=" + status +
           ", mmeId=" + std::to_string(mmeId) + "]";
  }
};
} // namespace server
