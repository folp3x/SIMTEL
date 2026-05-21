#pragma once

#include <string>

#include "common/types.h"

namespace common {
// базовый класс для хранения конфигурации
class Config {
private:
  static constexpr int INVALID_PORT = -1;

  bool locationSet = false;

  std::string ip = "";
  int port = INVALID_PORT;
  common::coords_t<> loc = {0, 0, 0};

public:
  virtual ~Config() = default;

  virtual bool isInitialized() const;

  std::string getIP() const;
  void setIP(const std::string &ip_);

  int getPort() const;
  void setPort(int port_);

  common::coords_t<> getLoc() const;
  void setLoc(const common::coords_t<> &loc_);
};
} // namespace common
