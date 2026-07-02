#include "simtel_pcrf.h"

namespace server {
std::string SimtelPcrf::createLogMsg(const std::string &content) const {
  return "PCRF: " + content;
}

SimtelPcrf::SimtelPcrf(
    double smsPriceRub_,
    const std::unordered_map<common::imsi_t, BalanceInfo> &balanceInfo_)
    : smsPriceRub(smsPriceRub_), balanceInfo(balanceInfo_) {}
} // namespace server
