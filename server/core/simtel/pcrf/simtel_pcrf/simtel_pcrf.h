#pragma once

#include <optional>
#include <unordered_map>

#include "server/core/simtel/pcrf/balance_info/balance_info.h"

namespace server {
class SimtelPcrf {
private:
  const double smsPriceRub;

  std::unordered_map<common::imsi_t, BalanceInfo> balanceInfo;

  std::string createLogMsg(const std::string &content) const;

  std::optional<BalanceInfo> findByImsi(const common::imsi_t &imsi) const;

public:
  SimtelPcrf(
      double smsPriceRub_,
      const std::unordered_map<common::imsi_t, BalanceInfo> &balanceInfo_);

  std::optional<bool> hasEnoughBalanceForSms(const common::imsi_t &imsi);

  bool reserveMoneyForSms(const common::imsi_t &imsi);
  bool returnReservedMoney(const common::imsi_t &imsi);
  bool deductReservedMoney(const common::imsi_t &imsi);

  std::optional<double> getBalance(const common::imsi_t &imsi);
};
} // namespace server
