#pragma once

#include "common/app/config/config/config.h"

#include <vector>

#include "app/config/mme_config/mme_config.h"
#include "app/config/pcrf_config/pcrf_config.h"
#include "app/config/smsc_config/smsc_config.h"

namespace server {
class Config : public common::Config {
private:
  MmeConfig curMmeConfig{};
  BalanceInfo curBalanceInfo{};

  std::vector<MmeConfig> mmeConfigs{};
  SmscConfig smscConfig{};
  PcrfConfig pcrfConfig{};

  std::string bsFilePath = "";
  std::string epcFilePath = "";

  virtual std::unique_ptr<common::BaseJsonInfo> getJsonRootInfo() override;

public:
  std::string getBsFilePath() const;
  std::string getEpcFilePath() const;

  std::vector<MmeConfig> getMmeConfigs() const;

  SmscConfig getSmscConfig() const;
  PcrfConfig getPcrfConfig() const;
};
} // namespace server
