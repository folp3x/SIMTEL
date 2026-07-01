#pragma once

#include "common/types.h"

namespace server {
struct HlrRecord {
  int id;
  common::imsi_t imsi;
  common::msisdn_t msisdn;
  unsigned int mmeId = 0;

  std::string toStr() const {
    return "[id=" + std::to_string(id) + ", imsi=" + imsi +
           ", msisdn=" + msisdn + ", mmeId=" + std::to_string(mmeId) + "]";
  }
};
} // namespace server
