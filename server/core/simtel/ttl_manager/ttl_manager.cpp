#include "ttl_manager.h"

namespace server {
TtlManager &TtlManager::instance() {
  static TtlManager manager;
  return manager;
}

void TtlManager::setTimeout(unsigned int timeoutSec) {
  timeout = std::chrono::seconds{timeoutSec};
}

void TtlManager::update() { lastActivity = std::chrono::steady_clock::now(); }

bool TtlManager::isExpired() const {
  return std::chrono::steady_clock::now() - lastActivity >= timeout;
}
} // namespace server
