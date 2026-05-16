#pragma once

#include "common/app/config/config/config.h"
#include "common/types.h"

namespace client {
class Config : public common::Config {
private:
  common::imei_t imei = "";
  common::imsi_t imsi = "";

public:
  virtual bool isInitialized() const override;

  std::string getImei() const;
  void setImei(const common::imei_t &imei_);

  std::string getImsi() const;
  void setImsi(const common::imsi_t &imsi_);
};
} // namespace client
