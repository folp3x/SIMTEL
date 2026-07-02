#include "simtel_pcrf.h"

#include "server/app/message_holder/message_holder.h"

namespace server {
std::string SimtelPcrf::createLogMsg(const std::string &content) const {
  return "PCRF: " + content;
}

std::optional<BalanceInfo>
SimtelPcrf::findByImsi(const common::imsi_t &imsi) const {
  auto it = balanceInfo.find(imsi);
  if (it == balanceInfo.end()) {
    return std::nullopt;
  }

  return it->second;
}

SimtelPcrf::SimtelPcrf(
    double smsPriceRub_,
    const std::unordered_map<common::imsi_t, BalanceInfo> &balanceInfo_)
    : smsPriceRub(smsPriceRub_), balanceInfo(balanceInfo_) {}

std::optional<bool>
SimtelPcrf::hasEnoughBalanceForSms(const common::imsi_t &imsi) {
  auto info = findByImsi(imsi);
  if (!info) {
    return false;
  }

  double balance = info->balanceRub;

  MessageHolder::instance().addMsg(
      createLogMsg("checked balance: " + info->toStr()));

  return balance >= smsPriceRub;
}

bool SimtelPcrf::reserveMoneyForSms(const common::imsi_t &imsi) {
  auto it = balanceInfo.find(imsi);
  if (it == balanceInfo.end()) {
    return false;
  }

  it->second.reservedRub = smsPriceRub;
  it->second.balanceRub -= smsPriceRub;

  MessageHolder::instance().addMsg(
      createLogMsg("reserved balance: " + it->second.toStr()));

  return true;
}

bool SimtelPcrf::returnReservedMoney(const common::imsi_t &imsi) {
  auto it = balanceInfo.find(imsi);
  if (it == balanceInfo.end()) {
    return false;
  }

  it->second.balanceRub += it->second.reservedRub;
  it->second.reservedRub = 0;

  MessageHolder::instance().addMsg(
      createLogMsg("returned reserved money: " + it->second.toStr()));

  return true;
}

bool SimtelPcrf::deductReservedMoney(const common::imsi_t &imsi) {
  auto it = balanceInfo.find(imsi);
  if (it == balanceInfo.end()) {
    return false;
  }

  it->second.reservedRub = 0;

  MessageHolder::instance().addMsg(
      createLogMsg("deducted reserved money: " + it->second.toStr()));

  return true;
}

std::optional<double> SimtelPcrf::getBalance(const common::imsi_t &imsi) {
  auto info = findByImsi(imsi);
  if (!info) {
    return std::nullopt;
  }

  return info->balanceRub;
}
} // namespace server
