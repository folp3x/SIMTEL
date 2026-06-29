#include "ttl_manager.h"

namespace server {
TtlManager::TtlManager(unsigned int timeoutSec, unsigned int warningPeriod_)
    : timeout(timeoutSec), warningPeriod(warningPeriod_) {}

bool TtlManager::isExpired() const {
  std::lock_guard lock(lastActivityMtx);
  return std::chrono::steady_clock::now() - lastActivity >= timeout;
}

void TtlManager::start() {
  active = true;
  {
    std::lock_guard lock(lastActivityMtx);
    lastActivity = std::chrono::steady_clock::now();
  }
  lastWarningNum = 0;
}

void TtlManager::stop() { active = false; }

unsigned int TtlManager::getLeftSec() const {
  std::chrono::nanoseconds passed;
  auto now = std::chrono::steady_clock::now();
  {
    std::lock_guard lock(lastActivityMtx);
    passed = now - lastActivity;
  }
  auto passedSec = std::chrono::duration_cast<std::chrono::seconds>(passed);

  if (passedSec >= timeout) {
    return 0;
  }
  auto left = timeout - passedSec;

  return left.count();
}

bool TtlManager::isActive() const { return active.load(); }

std::optional<unsigned int> TtlManager::getWarningSec() {
  unsigned int leftSec = getLeftSec();
  unsigned int warningNum = (leftSec + warningPeriod - 1) / warningPeriod;

  if (warningNum > 0 && warningNum != lastWarningNum) {
    lastWarningNum = warningNum;
    return leftSec;
  }

  return std::nullopt;
}
} // namespace server
