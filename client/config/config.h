#pragma once

#include "common/app/config/config.h"

namespace client {
class Config : public common::Config {
private:
  friend class CLIParser;
  friend class ConfigParser;

  std::string imei = "";
  std::string imsi = "";

public:
  virtual bool isInitialized() const override;

  std::string getImei() const;
  void setImei(const std::string &imei_);

  std::string getImsi() const;
  void setImsi(const std::string &imsi_);
};
} // namespace client
