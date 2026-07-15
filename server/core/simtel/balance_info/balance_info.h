#pragma once

#include "common/types.h"

#include "common/utils/str/str.h"

namespace server {
struct BalanceInfo {
  common::imsi_t imsi = "";
  double balanceRub = 0;
  double reservedRub = 0;

  std::string toStr() const {
    std::string balanceStr = common::utils::toStr(
        balanceRub, common::constants::PriceNumPrecision, true);
    std::string reserverStr = common::utils::toStr(
        reservedRub, common::constants::PriceNumPrecision, true);

    return "[imsi=" + imsi + ", balance = " + balanceStr +
           " rub, reserved = " + reserverStr + " rub]";
  }
};
} // namespace server
