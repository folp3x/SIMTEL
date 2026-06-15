#pragma once

#include <memory>
#include <optional>

#include "common/types.h"

namespace server {
class SimtelBaseStation;

struct VlrRecord {
  common::imsi_t mTimsi = "";
  common::imei_t imei = "";
  common::msisdn_t msisdn = "";
  std::shared_ptr<SimtelBaseStation> bs;

  std::string toStr() const;
};
} // namespace server
