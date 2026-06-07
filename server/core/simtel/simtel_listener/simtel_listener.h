#pragma once

#include <functional>

#include "server/network/socket/socket.h"

namespace server {
class SimtelListener {
private:
  std::unique_ptr<Socket> sock;

  static constexpr int MAX_CLIENT_THREADS = 20;

  std::atomic<int> activeThreads{0};

  std::function<void(const std::unique_ptr<Socket> &clientSock)> handler;

public:
  SimtelListener(
      const common::NetworkAddress &addr,
      const std::function<void(const std::unique_ptr<Socket> &clientSock)>
          &handler_);

  void handleClients();
};
} // namespace server
