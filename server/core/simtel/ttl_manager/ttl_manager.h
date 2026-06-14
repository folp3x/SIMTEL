#pragma once

#include <chrono>

namespace server {
class TtlManager {
private:
  std::chrono::steady_clock::time_point lastActivity;
  std::chrono::seconds timeout{};

public:
  static TtlManager &instance();

  void setTimeout(unsigned int timeoutSec);

  void update();

  bool isExpired() const;
};
} // namespace server
