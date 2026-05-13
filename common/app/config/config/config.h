#pragma once

#include <array>
#include <string>

#include "common/constants/constants.h"
#include "common/types/types.h"

namespace common {
// базовый класс для хранения конфигурации
class Config {
private:
  bool locationSet = false;

  std::string ip = "";
  int port = -1;
  common::coords_t<float> loc = {0, 0, 0};

public:
  virtual ~Config() = default;

  virtual bool isInitialized() const;

  std::string getIP() const;
  void setIP(const std::string &ip_);

  int getPort() const;
  void setPort(int port_);

  common::coords_t<float> getLoc() const;
  void setLoc(const common::coords_t<float> &loc_);
};
} // namespace common
