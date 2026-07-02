#pragma once

#include "common/types.h"

namespace server {
struct BalanceInfo {
  common::imsi_t imsi = "";
  double balanceRub = 0;
  double reservedRub = 0;
};
} // namespace server
