#pragma once

#include <unordered_map>

#include "server/core/simtel/balance_info/balance_info.h"

namespace server {
class SimtelPcrf {
private:
  const double smsPriceRub;

  std::unordered_map<common::imsi_t, BalanceInfo> balanceInfo;

  std::string createLogMsg(const std::string &content) const;

public:
  SimtelPcrf(
      double smsPriceRub_,
      const std::unordered_map<common::imsi_t, BalanceInfo> &balanceInfo_);
};
} // namespace server
