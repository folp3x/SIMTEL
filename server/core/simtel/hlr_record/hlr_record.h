#pragma once

#include "common/types.h"

namespace server {
struct HlrRecord {
  int id;
  common::imei_t imei;
  common::imsi_t imsi;
  common::msisdn_t msisdn;
  std::string status;
  std::optional<unsigned int> mmeId;

  std::string toStr() const {
    std::string mmeIdStr =
        mmeId.has_value() ? std::to_string(mmeId.value()) : "?";
    return "[id=" + std::to_string(id) + ", imsi=" + imsi + ", imei=" + imei +
           ", msisdn=" + msisdn + ", status=" + status + ", mmeId=" + mmeIdStr +
           "]";
  }
};
} // namespace server
