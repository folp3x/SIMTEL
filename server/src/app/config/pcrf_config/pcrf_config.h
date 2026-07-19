#pragma once

#include <unordered_map>

#include "core/simtel/pcrf/balance_info/balance_info.h"

namespace server {
struct PcrfConfig {
  double smsPriceRub = 0;
  std::unordered_map<common::imsi_t, BalanceInfo> balanceInfo{};
};
} // namespace server
