#include "simtel_listener.h"

namespace server {
SimtelListener::SimtelListener(const common::NetworkAddress &addr,
                               size_t maxUeThreads_)
    : maxUeThreads(maxUeThreads_) {
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

void SimtelListener::setTtlManager(std::shared_ptr<TtlManager> ttlManager_) {
  ttlManager = ttlManager_;
}

void SimtelListener::acceptConnections(
    const std::function<void(std::shared_ptr<SimtelUeContext> ctx)> &handler) {
  if (!ttlManager) {
    throw std::runtime_error("TTL manager not set");
  }

  listening = true;
  while (listening) {
    auto acceptResult = sock->acceptConnection();
    if (!acceptResult) {
      continue;
    }

    MessageHolder::instance().addMsg("\nUE_" + (*acceptResult)->getAddrStr() +
                                         " connected",
                                     common::MenuMessageType::INFO);

    if (activeThreads >= maxUeThreads) {
      MessageHolder::instance().addErrorMsg("Too many connections. UE ignored");
      continue;
    }

    ttlManager->update();

    auto ctx = std::make_shared<SimtelUeContext>(std::move(*acceptResult));
    std::thread singleClientHandler{[this, handler, ctx]() {
      activeThreads++;
      handler(ctx);
      activeThreads--;
      ttlManager->setActive(true);
    }};

    singleClientHandler.detach();
  }
}

void SimtelListener::stop() {
  listening = false;
  sock->closeSock();
}
} // namespace server
