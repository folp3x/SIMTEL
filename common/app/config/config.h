#pragma once

#include <array>
#include <string>

#include "common/constants.h"

namespace common {
// базовый класс для хранения конфигурации
class Config {
private:
  bool locationSet = false;

  std::string ip = "";
  int port = -1;
  std::array<double, constants::LOCATION_COORDS_COUNT> loc = {0, 0, 0};

public:
  virtual ~Config() = default;

  virtual bool isInitialized() const;

  std::string getIP() const;
  void setIP(const std::string &ip_);

  int getPort() const;
  void setPort(int port_);

  std::array<double, constants::LOCATION_COORDS_COUNT> getLoc() const;
  void setLoc(const std::array<double, constants::LOCATION_COORDS_COUNT> &loc_);
};
} // namespace common
