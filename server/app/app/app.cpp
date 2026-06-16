#include "app.h"

#include <csignal>
#include <thread>

#include "common/app/signals/signal_handler/signal_handler.h"
#include "server/core/simtel/simtel_base_station/simtel_base_station.h"

namespace server {
void App::sigintHandler(int signal) {
  if (signal == SIGINT) {
    std::cout << std::endl;
    exitApp();
    std::exit(signal);
  }
}

void App::exitApp() {
  isRunning = false;
  listener.stop();

  if (common::Logger::isInitialized()) {
    common::Logger::instance().getInner()->flush();
  }

  std::cout << "Exiting app..." << std::endl;
}

App::App(const common::NetworkAddress &addr, size_t maxUeThreadsCount,
         const std::vector<MmeConfig> &mmeConfigs, const SmscConfig &smscConfig,
         const std::vector<BsConfig> &bsConfigs, const EpcConfig &epcConfig)
    : ttlManager(std::make_shared<TtlManager>(epcConfig.ttlSec,
                                              TTL_WARNING_PERIOD_SEC)),
      listener(addr, maxUeThreadsCount),
      hlr(std::make_shared<SimtelRegister>(epcConfig.hlrSqliteFilePath)),
      smsc(std::make_unique<SimtelSmsc>(smscConfig)) {
  SimtelBaseStation::setTtlManager(ttlManager);
  listener.setTtlManager(ttlManager);

  for (const auto &config : mmeConfigs) {
    mmeList.insert(
        {config.id, std::make_shared<SimtelMme>(config, hlr, smsc.get())});
  }

  for (auto &[id, mme] : mmeList) {
    for (auto &[otherId, otherMme] : mmeList) {
      if (id != otherId) {
        mme->addOtherMme(otherMme);
      }
    }
  }

  for (const auto &config : bsConfigs) {
    auto it = mmeList.find(config.mmeId);
    if (it == mmeList.end()) {
      throw std::runtime_error("Unknown MME id in BS config");
    }

    auto bs = std::make_shared<SimtelBaseStation>(config, it->second.get());
    it->second->addBs(bs);
    SimtelBaseStation::addBs(bs);
  }

  if (!hlr->hasData()) {
    hlr->insertData();
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

  ttlManager->update();
  ttlManager->setActive(true);

  while (isRunning) {
    if (ttlManager->isActive() && ttlManager->isExpired()) {
      break;
    }

    auto warningSec = ttlManager->getWarningSec();
    if (warningSec) {
      menu.showMessage({"TTL: " + std::to_string(*warningSec) + " seconds left",
                        common::MenuMessageType::INFO});
    }

    while (true) {
      auto msg = MessageHolder::instance().takeMsg();
      if (msg) {
        menu.showMessage(*msg);
      } else {
        break;
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(MENU_SLEEP_MS * 10));
  }

  exitApp();

  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(), "App exited");
}
} // namespace server
