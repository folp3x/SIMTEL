#pragma once

#include <string>

namespace common {
class Config {
private:
  static constexpr int InvalidPort = -1;

  int port = InvalidPort;

public:
  virtual ~Config() = default;

  virtual bool isInitialized() const;

  int getPort() const;
  void setPort(int port_);
};
} // namespace common
