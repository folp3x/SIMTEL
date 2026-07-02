#include "config.h"

namespace server {
std::string Config::getBsFilePath() const { return bsFilePath; }

void Config::setBsFilePath(const std::string &bsFilePath_) {
  bsFilePath = bsFilePath_;
}

std::string Config::getEpcFilePath() const { return epcFilePath; }

void Config::setEpcFilePath(const std::string &epcFilePath_) {
  epcFilePath = epcFilePath_;
}

std::vector<MmeConfig> Config::getMmeConfigs() const { return mmeConfigs; }

void Config::addMmeConfig(const MmeConfig &config) {
  mmeConfigs.push_back(config);
}

SmscConfig Config::getSmscConfig() const { return smscConfig; }

PcrfConfig Config::getPcrfConfig() const { return pcrfConfig; }

void Config::setSmscTtlMs(unsigned int smscTtlMs) {
  smscConfig.smsTtlMs = smscTtlMs;
}

void Config::setPcrfSmsPriceRub(double pcrfSmsPriceRub) {
  pcrfConfig.smsPriceRub = pcrfSmsPriceRub;
}

void Config::addPcrfBalanceInfo(const BalanceInfo &info) {
  pcrfConfig.balanceInfo.emplace(info.imsi, info);
}
} // namespace server
