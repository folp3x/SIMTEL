#include "app.h"

#include "common/app/menu/menu_item/menu_item_exit/menu_item_exit.h"
#include "common/app/menu/menu_item/menu_item_invalid/menu_item_invalid.h"
#include "common/logging/logger/logger.h"
#include "server/app/menu/menu/menu.h"
#include "server/app/menu/menu_item/menu_item_dist/menu_item_dist.h"
#include "server/core/distance_calculator/distance_calculator.h"

namespace server {
App::App(const common::Location<> &location_,
         const common::NetworkAddress &addr_)
    : common::App<Config>(location_), addr(addr_) {
  auto createResult = Socket::create(addr_);
  if (!createResult) {
    throw std::runtime_error("Error creating socket: " + createResult.error());
  }

  sock = std::move(*createResult);
  auto error = sock->listenForConnections();
  if (error) {
    throw std::runtime_error("Error listening for connections: " + *error);
  }
}

App::Messages App::handleCommand(const std::unique_ptr<common::MenuItem> &cmd,
                                 bool &exit) {
  Messages messages = {};
  // выполнение команды в засимости от ее типа
  if (auto *invalidCmd = dynamic_cast<common::MenuItemInvalid *>(cmd.get())) {
    messages.push_back({"Error! " + invalidCmd->getError()});
  } else if (dynamic_cast<common::MenuItemExit *>(cmd.get())) {
    messages.push_back({"Exiting app..."});
    exit = true;
  } else if (auto *distCmd = dynamic_cast<MenuItemDist<> *>(cmd.get())) {
    auto coords = distCmd->getCoords();
    auto dist = DistanceCalculator::calc<decltype(coords)>(location, coords);
    messages.push_back({"Distance: " + common::toStr<decltype(dist)>(dist)});
  }

  return messages;
}

void App::run() {
  Menu menu;

  bool isRunning = true;

  while (isRunning) {
    menu.showMenuHeaderLine();
    menu.showStatus(location);
    menu.showMenuHeaderLine();
    menu.showCommandsInfo(getCommandsInfo());

    while (true) {
      auto acceptResult = sock->acceptConnection();
      if (!acceptResult) {
        continue;
      }

      auto clientSock = std::move(*acceptResult);
      SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                         "Client with addr={} connected",
                         clientSock->getAddrStr());

      common::Protocol clientProtocol;
      auto receiveResult = clientSock->receiveLocation(clientProtocol);
      if (!receiveResult) {
        SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                           "Error receiving location from client: {}",
                           receiveResult.error());
      }

      common::Location clientLocation = *receiveResult;
      SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                         "Location received from client: {}",
                         clientLocation.toStr());

      float distance = DistanceCalculator::calc(location, clientLocation);
      auto sendError = clientSock->sendDistance(clientProtocol, distance);
      if (sendError) {
        SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                           "Error sending distance to client:  {}", *sendError);
        continue;
      }

      SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                         "Distance sent to client: {}",
                         common::toStr(distance));
    }
  }
}
} // namespace server
