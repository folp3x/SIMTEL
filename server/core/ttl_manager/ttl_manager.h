#pragma once

#include <atomic>
#include <chrono>
#include <mutex>

namespace server {
class TtlManager {
private:
  mutable std::mutex lastActivityMtx;
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
