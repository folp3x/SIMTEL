#pragma once

#include <atomic>
#include <chrono>
#include <mutex>
#include <optional>

namespace server {
class TtlManager {
private:
  const unsigned int warningPeriod = 0;
  std::chrono::seconds timeout{};

  mutable std::mutex lastActivityMtx;
  std::chrono::steady_clock::time_point lastActivity;
  std::atomic<bool> active{false};
  unsigned int lastWarningNum = 0;

  unsigned int getLeftSec() const;

public:
  TtlManager(unsigned int timeoutSec, unsigned int warningPeriod_);

  void update();

  bool isExpired() const;

  void setActive(bool isActive);
  bool isActive() const;

  std::optional<unsigned int> getWarningSec();
};
} // namespace server
