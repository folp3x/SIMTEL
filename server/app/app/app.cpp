#include "app.h"

#include <csignal>
#include <thread>

#include "common/app/signals/signal_handler/signal_handler.h"
#include "server/core/simtel/simtel_base_station/simtel_base_station.h"

namespace server {
void App::sigintHandler(int signal) {
  if (signal == SIGINT) {
    listener.stop();
    isRunning = false;

    if (common::Logger::isInitialized()) {
      common::Logger::instance().getInner()->flush();
    }

    std::cout << std::endl << "Exiting app..." << std::endl;
    std::exit(signal);
  }
}

App::App(const common::NetworkAddress &addr, size_t maxUeThreadsCount,
         const std::vector<MmeConfig> &mmeConfigs, const SmscConfig &smscConfig,
         const std::vector<BsConfig> &bsConfigs, const EpcConfig &epcConfig)
    : listener(addr, maxUeThreadsCount) {
  for (const auto &config : bsConfigs) {
    SimtelBaseStation::addBs(std::make_unique<SimtelBaseStation>(config));
  }

  common::SignalHandler::setHandler(
      SIGINT, [this](int signal) { sigintHandler(signal); });
}

void App::run() {
  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(), "App started");

  isRunning = true;

  menu.showStatus();
  std::jthread connectionHandler{[this]() {
    listener.acceptConnections([](std::shared_ptr<SimtelUeContext> ctx) {
      SimtelBaseStation::handleConnectionRequest(ctx);
    });
  }};

  while (isRunning) {
    std::this_thread::sleep_for(std::chrono::milliseconds(MENU_SLEEP_MS));

    while (true) {
      auto msg = MessageHolder::instance().takeMsg();
      if (msg) {
        menu.showMessage(*msg);
      } else {
        break;
      }
    }
  }

  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(), "App exited");
}
} // namespace server
