#pragma once

#include <atomic>
#include <chrono>

namespace server {
class TtlManager {
private:
  std::chrono::steady_clock::time_point lastActivity;
  std::chrono::seconds timeout{};
  std::atomic<bool> active{false};

public:
  explicit TtlManager(unsigned int timeoutSec);

  void update();

  bool isExpired() const;

  void setActive(bool isActive);
  bool isActive() const;
};
} // namespace server
