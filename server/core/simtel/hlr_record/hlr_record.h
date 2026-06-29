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
  common::imsi_t mTimsi = "";

  std::string toStr() const {
    std::string mmeIdStr = std::to_string(mmeId);
    std::string mTimsiStr = isMtimsiSet() ? mTimsi : "?";
    return "[id=" + std::to_string(id) + ";imsi=" + imsi + ";imei=" + imei +
           ";msisdn=" + msisdn + ";status=" + status + ";mmeId=" + mmeIdStr +
           ";mTimsi=" + mTimsiStr + "]";
  }

  bool isMtimsiSet() const { return !mTimsi.empty(); }
};
} // namespace server
