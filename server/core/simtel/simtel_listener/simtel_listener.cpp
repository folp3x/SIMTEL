#include "simtel_listener.h"

#include <iostream>

#include "common/logging/logger/logger.h"

namespace server {
SimtelListener::SimtelListener(
    const common::NetworkAddress &addr,
    const std::function<void(std::shared_ptr<SimtelUeContext> ctx)> &handler_,
    size_t maxUeThreads_)
    : handler(handler_), maxUeThreads(maxUeThreads_) {
  auto createResult = Socket::create(addr);
  if (!createResult) {
    throw std::runtime_error("Error creating socket: " + createResult.error());
  }

  sock = std::move(*createResult);
  auto error = sock->listenForConnections();
  if (error) {
    throw std::runtime_error("Error listening for connections: " + *error);
  }
}

void SimtelListener::acceptConnections() {
  listening = true;
  while (listening) {
    auto acceptResult = sock->acceptConnection();
    if (!acceptResult) {
      continue;
    }

    std::cout << "\nUE connected: " << (*acceptResult)->getAddrStr()
              << std::endl;

    if (activeThreads >= maxUeThreads) {
      std::cout << "Too many connections. UE ignored" << std::endl;
      continue;
    }

    auto ctx = std::make_shared<SimtelUeContext>(std::move(*acceptResult));
    std::thread singleClientHandler{[this, ctx]() {
      activeThreads++;
      handler(ctx);
      activeThreads--;
    }};
    singleClientHandler.detach();
  }
}

void SimtelListener::stop() {
  listening = false;
  sock->closeSock();
}
} // namespace server
