#pragma once

#include <array>
#include <string>

#include "common/constants.h"

class Config {
private:
  // для парсинга значений напрямую в поля класса
  friend class CLIParser;

  bool locationSet = false;

  std::string ip = "";
  int port = -1;
  std::string imei = "";
  std::string imsi = "";
  std::array<double, Constants::LOCATION_COORDS_COUNT> loc = {0, 0, 0};

public:
  bool isInitialized() const;

  std::string getIP() const;
  void setIP(const std::string &ip_);

  int getPort() const;
  void setPort(int port_);

  std::string getImei() const;
  void setImei(const std::string &imei_);

  std::string getImsi() const;
  void setImsi(const std::string &imsi_);

  std::array<double, Constants::LOCATION_COORDS_COUNT> getLoc() const;
  void setLoc(const std::array<double, Constants::LOCATION_COORDS_COUNT> &loc_);
};
