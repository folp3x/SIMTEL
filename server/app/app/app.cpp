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
         const common::NetworkAddress &addr)
    : location(location_),
      listener(addr, [this](const std::unique_ptr<Socket> &clientSock) {
        handleSingleClient(clientSock);
      }) {
  std::signal(SIGINT, sigintHandler);
}

void App::handleSingleClient(const std::unique_ptr<Socket> &clientSock) {
  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                     "Client with addr={} connected", clientSock->getAddrStr());
  common::Protocol clientProtocol;
  auto receiveResult = clientSock->receiveLocation(clientProtocol);
  if (!receiveResult) {
    SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                       "Error receiving location from client: {}",
                       receiveResult.error());
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
    return;
  }

  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                     "Distance sent to client: {}", common::toStr(distance));
}

void App::run() {
  Menu menu;
  messages = {};

  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(), "App started");

  std::thread clientsHandler{[this]() { listener.handleClients(); }};

  while (true) {
    menu.showMenuHeaderLine();
    menu.showStatus();
    menu.showMenuHeaderLine();

    menu.showMessages(messages);
  }

  clientsHandler.join();

  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(), "App exited");
}
} // namespace server
