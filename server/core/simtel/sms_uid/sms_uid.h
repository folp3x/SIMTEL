#pragma once

#include "common/types.h"

namespace server {
struct SmsUid {
  common::imsi_t mTimsi = "";
  unsigned int smsId = 0;

  std::string toStr() const {
    return "sms_uid{mTimsi=" + mTimsi + ", smsId=" + std::to_string(smsId) +
           "}";
  }

  bool operator<(const SmsUid &other) const {
    if (mTimsi != other.mTimsi) {
      return mTimsi < other.mTimsi;
    }
    return smsId < other.smsId;
  }
};
} // namespace server
