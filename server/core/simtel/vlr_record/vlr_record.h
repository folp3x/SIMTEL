#pragma once

#include <memory>
#include <optional>

#include "common/types.h"

namespace server {
class SimtelBaseStation;

struct VlrRecord {
  common::imsi_t imsi = "";
  common::imei_t imei = "";
  common::msisdn_t msisdn = "";
  std::shared_ptr<SimtelBaseStation> bs = nullptr;

  std::string toStr() const;
};
} // namespace server
