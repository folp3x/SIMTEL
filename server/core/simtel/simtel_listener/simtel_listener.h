#pragma once

#include <functional>

#include "server/core/simtel/simtel_ue_context/simtel_ue_context.h"
#include "server/network/socket/socket.h"

namespace server {
class SimtelListener {
private:
  const size_t maxUeThreads;

  bool listening = false;

  std::unique_ptr<Socket> sock;

  std::atomic<int> activeThreads{0};

  std::function<void(std::shared_ptr<SimtelUeContext> ctx)> handler;

public:
  SimtelListener(
      const common::NetworkAddress &addr,
      const std::function<void(std::shared_ptr<SimtelUeContext> ctx)> &handler_,
      size_t maxUeThreads_);

  void acceptConnections();

  void stop();
};
} // namespace server
