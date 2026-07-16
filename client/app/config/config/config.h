#pragma once

#include "common/app/config/config/config.h"
#include "common/types.h"

namespace client {
class Config : public common::Config {
private:
  common::imei_t imei = "";
  common::imsi_t imsi = "";

  std::string ip = "";

  std::string addressBookFilePath = "";

  common::coords_t<> loc = {0};

  virtual std::unique_ptr<common::BaseJsonInfo> getJsonRootInfo() override;

public:
  std::string getImei() const;
  void setImei(const common::imei_t &imei_);

  std::string getImsi() const;
  void setImsi(const common::imsi_t &imsi_);

  std::string getIp() const;
  void setIp(const std::string &ip_);

  std::string getAddressBookFilePath() const;

  common::coords_t<> getLoc() const;
  void setLoc(const common::coords_t<> &loc_);
};
} // namespace client
