#pragma once

#include "common/app/config/config/config.h"

#include "server/app/config/mme_config/mme_config.h"
#include "server/app/config/smsc_config/smsc_config.h"

namespace server {
class Config : public common::Config {
private:
  MmeConfig mmeConfig{};
  SmscConfig smscConfig{};

  std::string bsFilePath = "";
  std::string epcFilePath = "";

public:
  virtual bool isInitialized() const override;

  std::string getBsFilePath() const;
  void setBsFilePath(const std::string &bsFilePath_);

  std::string getEpcFilePath() const;
  void setEpcFilePath(const std::string &epcFilePath_);

  size_t getMmeMaxVlrSize() const;
  void setMmeMaxVlrSize(size_t maxVlrSize);

  unsigned int getSmscTtlMs() const;
  void setSmscTtlMs(unsigned int smscTtlMs);

  std::string getSmsCdrAccessJsonFilePath() const;
  void setSmsCdrAccessJsonFilePath(const std::string &smscCdrAccessFilePath_);
};
} // namespace server
