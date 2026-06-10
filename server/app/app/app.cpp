#include "app.h"

#include <csignal>
#include <thread>

#include "common/app/signals/signal_handler/signal_handler.h"
#include "common/logging/logger/logger.h"
#include "server/core/distance_calculator/distance_calculator.h"

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

App::App(const common::NetworkAddress &addr)
    : listener(addr, [this](std::shared_ptr<SimtelUeContext> ctx) {
        SimtelBaseStation::handleConnectionRequest(ctx);
      }) {
  common::SignalHandler::setHandler(
      SIGINT, [this](int signal) { sigintHandler(signal); });
}

void App::run() {
  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(), "App started");

  menu.showStatus();
  listener.handleClients();

  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(), "App exited");
}
} // namespace server
