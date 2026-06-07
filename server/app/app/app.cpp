#include "app.h"

#include <csignal>
#include <thread>

#include "common/app/menu/menu_item/menu_item_exit/menu_item_exit.h"
#include "common/app/menu/menu_item/menu_item_invalid/menu_item_invalid.h"
#include "common/logging/logger/logger.h"
#include "server/app/menu/menu/menu.h"
#include "server/core/distance_calculator/distance_calculator.h"

namespace server {
void App::sigintHandler(int signal) {
  if (signal == SIGINT) {
    if (common::Logger::isInitialized()) {
      common::Logger::instance().getInner()->flush();
    }

    std::cout << "Exiting app..." << std::endl;
    std::exit(signal);
  }
}

App::App(const common::Location<> &location_,
         const common::NetworkAddress &addr_)
    : location(location_), addr(addr_) {
  auto createResult = Socket::create(addr_);
  if (!createResult) {
    throw std::runtime_error("Error creating socket: " + createResult.error());
  }

  sock = std::move(*createResult);
  auto error = sock->listenForConnections();
  if (error) {
    throw std::runtime_error("Error listening for connections: " + *error);
  }

  std::signal(SIGINT, sigintHandler);
}

void App::handleSingleClient(const std::unique_ptr<Socket> &clientSock) {
  activeClientThreads++;
  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                     "Client with addr={} connected", clientSock->getAddrStr());
  common::Protocol clientProtocol;
  auto receiveResult = clientSock->receiveLocation(clientProtocol);
  if (!receiveResult) {
    SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                       "Error receiving location from client: {}",
                       receiveResult.error());
    activeClientThreads--;
    activeClientThreads.notify_one();
    return;
  }

  common::Location clientLocation = *receiveResult;
  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                     "Location received from client: {}",
                     clientLocation.toStr());

  float distance = DistanceCalculator::calc(location, clientLocation);
  auto sendError = clientSock->sendDistance(clientProtocol, distance);
  if (sendError) {
    SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                       "Error sending distance to client: {}", *sendError);
    activeClientThreads--;
    activeClientThreads.notify_one();
    return;
  }

  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                     "Distance sent to client: {}", common::toStr(distance));

  activeClientThreads--;
  activeClientThreads.notify_one();
}

void App::handleClients() {
  constexpr int MAX_CLIENT_THREADS = 20;

  while (isRunning) {
    auto acceptResult = sock->acceptConnection();
    if (!acceptResult) {
      continue;
    }

    if (activeClientThreads >= MAX_CLIENT_THREADS) {
      SPDLOG_LOGGER_WARN(common::Logger::instance().getInner(),
                         "Too many clients. {} rejected",
                         (*acceptResult)->getAddrStr());
      continue;
    }

    std::thread singleClientHandler{
        [this, clientSock = std::move(*acceptResult)]() {
          handleSingleClient(clientSock);
        }};
    singleClientHandler.detach();
  }

  // ожидание завершения обработки всех клиентов
  if (activeClientThreads.load() != 0) {
    activeClientThreads.wait(0);
  }
}

void App::run() {
  Menu menu;
  messages = {};
  isRunning = true;

  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(), "App started");

  std::thread clientsHandler{[this]() { handleClients(); }};

  while (isRunning) {
    menu.showMenuHeaderLine();
    menu.showStatus();
    menu.showMenuHeaderLine();

    menu.showMessages(messages);
  }

  sock->closeSock();
  clientsHandler.join();

  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(), "App exited");
}
} // namespace server
