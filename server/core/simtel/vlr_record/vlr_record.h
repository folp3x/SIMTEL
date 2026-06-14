#pragma once

#include <optional>

#include "common/types.h"

namespace server {
struct VlrRecord {
  common::imsi_t mTimsi = "";
  common::imei_t imei = "";
  common::msisdn_t msisdn = "";
  std::optional<unsigned int> bsId = std::nullopt;
};
} // namespace server
