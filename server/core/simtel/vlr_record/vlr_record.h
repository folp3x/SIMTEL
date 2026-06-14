#pragma once

#include "common/types.h"

namespace server {
struct VlrRecord {
  common::imsi_t mTimsi = "";
  common::imei_t imei = "";
  common::msisdn_t msisdn = "";
  unsigned int bsId = 0;
};
} // namespace server
