#include "simtel_listener.h"

#include "common/logging/logger/logger.h"

namespace server {
SimtelListener::SimtelListener(
    const common::NetworkAddress &addr,
    const std::function<void(const std::unique_ptr<Socket> &clientSock)>
        &handler_)
    : handler(handler_) {
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

void SimtelListener::handleClients() {
  while (listening) {
    auto acceptResult = sock->acceptConnection();
    if (!acceptResult) {
      continue;
    }

    if (activeThreads >= MAX_CLIENT_THREADS) {
      SPDLOG_LOGGER_WARN(common::Logger::instance().getInner(),
                         "Too many clients. {} rejected",
                         (*acceptResult)->getAddrStr());
      continue;
    }

    std::thread singleClientHandler{
        [this, clientSock = std::move(*acceptResult)]() {
          activeThreads++;
          handler(clientSock);
          activeThreads--;
        }};
    singleClientHandler.detach();
  }
}

void SimtelListener::stopListening() {
  listening = false;
  sock->closeSock();
}
} // namespace server
