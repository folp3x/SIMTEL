#pragma once

#include <functional>

#include "server/core/simtel/simtel_ue_context/simtel_ue_context.h"
#include "server/network/socket/socket.h"

namespace server {
class SimtelListener {
private:
  bool listening = true;

  std::unique_ptr<Socket> sock;

  static constexpr int MAX_CLIENT_THREADS = 20;

  std::atomic<int> activeThreads{0};

  std::function<void(std::shared_ptr<SimtelUeContext> ctx)> handler;

public:
  SimtelListener(const common::NetworkAddress &addr,
                 const std::function<void(std::shared_ptr<SimtelUeContext> ctx)>
                     &handler_);

  void handleClients();

  void stopListening();
};
} // namespace server
