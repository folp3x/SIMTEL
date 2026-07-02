#include "simtel_pcrf.h"

#include "common/utils/str/str.h"
#include "server/app/message_holder/message_holder.h"

namespace server {
std::string SimtelPcrf::createLogMsg(const std::string &content) const {
  return "PCRF: " + content;
}

SimtelPcrf::SimtelPcrf(
    double smsPriceRub_,
    const std::unordered_map<common::imsi_t, BalanceInfo> &balanceInfo_)
    : smsPriceRub(smsPriceRub_), balanceInfo(balanceInfo_) {}

std::optional<bool>
SimtelPcrf::hasEnoughBalanceForSms(const common::imsi_t &imsi) {
  auto it = balanceInfo.find(imsi);
  if (it == balanceInfo.end()) {
    return std::nullopt;
  }

  double balance = it->second.balanceRub;

  MessageHolder::instance().addMsg(
      createLogMsg("Balance of imsi=" + imsi +
                   " checked: " + common::toStr(balance) + "rub"));

  return balance >= smsPriceRub;
}

bool SimtelPcrf::reserveMoneyForSms(const common::imsi_t &imsi) {
  auto it = balanceInfo.find(imsi);
  if (it == balanceInfo.end()) {
    return false;
  }

  it->second.reservedRub = smsPriceRub;
  it->second.balanceRub -= smsPriceRub;

  return true;
}

bool SimtelPcrf::returnReservedMoney(const common::imsi_t &imsi) {
  auto it = balanceInfo.find(imsi);
  if (it == balanceInfo.end()) {
    return false;
  }

  it->second.balanceRub += it->second.reservedRub;
  it->second.reservedRub = 0;

  return true;
}

bool SimtelPcrf::deductReservedMoney(const common::imsi_t &imsi) {
  auto it = balanceInfo.find(imsi);
  if (it == balanceInfo.end()) {
    return false;
  }

  it->second.reservedRub = 0;

  return true;
}
} // namespace server
