#pragma once

#include <string>

namespace common {
// базовый класс для хранения конфигурации
class Config {
private:
  static constexpr int INVALID_PORT = -1;

  int port = INVALID_PORT;

public:
  virtual ~Config() = default;

  virtual bool isInitialized() const;

  int getPort() const;
  void setPort(int port_);
};
} // namespace common
