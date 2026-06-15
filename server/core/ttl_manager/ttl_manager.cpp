#include "ttl_manager.h"

namespace server {
TtlManager::TtlManager(unsigned int timeoutSec) : timeout(timeoutSec) {}

void TtlManager::update() {
  std::lock_guard lock(lastActivityMtx);
  lastActivity = std::chrono::steady_clock::now();
}

bool TtlManager::isExpired() const {
  std::lock_guard lock(lastActivityMtx);
  return std::chrono::steady_clock::now() - lastActivity >= timeout;
}

void TtlManager::setActive(bool isActive) { active = isActive; }

bool TtlManager::isActive() const { return active.load(); }
} // namespace server
