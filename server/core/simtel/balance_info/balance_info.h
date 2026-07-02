#pragma once

#include "common/types.h"

#include "common/utils/str/str.h"

namespace server {
struct BalanceInfo {
  common::imsi_t imsi = "";
  double balanceRub = 0;
  double reservedRub = 0;

  std::string toStr() const {
    return "[imsi=" + imsi +
           ", balance = " + common::toStr(balanceRub, 2, true) +
           " rub, reserved = " + common::toStr(reservedRub, 2, true) + " rub]";
  }
};
} // namespace server
