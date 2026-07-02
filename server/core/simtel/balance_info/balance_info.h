#pragma once

#include "common/types.h"

namespace server {
struct BalanceInfo {
  common::imsi_t imsi = "";
  double balance = 0;
};
} // namespace server
