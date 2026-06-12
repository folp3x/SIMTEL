#include "app.h"

#include <csignal>
#include <thread>

#include "common/app/signals/signal_handler/signal_handler.h"
#include "server/core/simtel/simtel_base_station/simtel_base_station.h"

namespace server {
void App::sigintHandler(int signal) {
  if (signal == SIGINT) {
    listener.stop();

    if (common::Logger::isInitialized()) {
      common::Logger::instance().getInner()->flush();
    }

    std::cout << std::endl << "Exiting app..." << std::endl;
    std::exit(signal);
  }
}

App::App(const common::NetworkAddress &addr, size_t maxUeThreads)
    : listener(addr, maxUeThreads, msgHolder) {
  SimtelBaseStation::addBs(std::make_unique<SimtelBaseStation>(
      1, 120, 10, common::Location<>{{-100}}));
  SimtelBaseStation::addBs(std::make_unique<SimtelBaseStation>(
      2, 120, 10, common::Location<>{{100}}));
  common::SignalHandler::setHandler(
      SIGINT, [this](int signal) { sigintHandler(signal); });
}

void App::run() {
  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(), "App started");

  menu.showStatus();
  std::jthread connectionHandler{[this]() {
    listener.acceptConnections([](std::shared_ptr<SimtelUeContext> ctx) {
      SimtelBaseStation::handleConnectionRequest(ctx);
    });
  }};

  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(), "App exited");
}
} // namespace server
