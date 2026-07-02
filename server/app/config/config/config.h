#pragma once

#include "common/app/config/config/config.h"

#include <vector>

#include "server/app/config/mme_config/mme_config.h"
#include "server/app/config/pcrf_config/pcrf_config.h"
#include "server/app/config/smsc_config/smsc_config.h"

namespace server {
class Config : public common::Config {
private:
  std::vector<MmeConfig> mmeConfigs{};
  SmscConfig smscConfig{};
  PcrfConfig pcrfConfig{};

  std::string bsFilePath = "";
  std::string epcFilePath = "";

public:
  std::string getBsFilePath() const;
  void setBsFilePath(const std::string &bsFilePath_);

  std::string getEpcFilePath() const;
  void setEpcFilePath(const std::string &epcFilePath_);

  std::vector<MmeConfig> getMmeConfigs() const;
  void addMmeConfig(const MmeConfig &config);

  SmscConfig getSmscConfig() const;
  void setSmscTtlMs(unsigned int smscTtlMs);

  PcrfConfig getPcrfConfig() const;
  void setPcrfSmsPriceRub(double pcrfSmsPriceRub);
  void addPcrfBalanceInfo(const BalanceInfo &info);
};
} // namespace server
