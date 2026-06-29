#pragma once

#include <functional>

#include "server/core/simtel/simtel_ue_context/simtel_ue_context.h"
#include "server/core/ttl_manager/ttl_manager.h"

namespace server {
class SimtelListener {
private:
  const size_t maxUeThreads;

  bool listening = false;

  std::unique_ptr<Socket> sock;

  std::atomic<int> activeThreads{0};

  std::shared_ptr<TtlManager> ttlManager;

public:
  SimtelListener(const common::NetworkAddress &addr, size_t maxUeThreads);

  void setTtlManager(std::shared_ptr<TtlManager> ttlManager_);

  void acceptConnections(
      const std::function<void(std::shared_ptr<SimtelUeContext> ctx)> &handler);

  void stop();
};
} // namespace server
