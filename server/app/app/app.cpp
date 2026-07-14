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
  running = false;
  listener.stop();

  std::cout << "Exiting app..." << std::endl;
}

App::App(const common::NetworkAddress &addr, size_t maxUeThreadsCount,
         const std::vector<MmeConfig> &mmeConfigs, const SmscConfig &smscConfig,
         const std::vector<BsConfig> &bsConfigs, const EpcConfig &epcConfig,
         const PcrfConfig &pcrfConfig)
    : ttlManager(
          std::make_shared<TtlManager>(epcConfig.ttlSec, TtlWarningPeriodSec)),
      listener(addr, maxUeThreadsCount),
      reg(std::make_shared<SimtelRegister>(epcConfig.hlrSqliteFilePath,
                                           epcConfig.eirSqliteFilePath)),
      smsc(std::make_unique<SimtelSmsc>(smscConfig)),
      pcrf(std::make_shared<SimtelPcrf>(pcrfConfig.smsPriceRub,
                                        pcrfConfig.balanceInfo)) {
  listener.setTtlManager(ttlManager);

  for (const auto &config : mmeConfigs) {
    mmeList.emplace(config.id,
                    std::make_shared<SimtelMme>(config, reg, smsc, pcrf));
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

    auto bs = std::make_shared<SimtelBaseStation>(config, it->second);
    it->second->addBs(bs);
    SimtelBaseStation::addBs(bs);
  }

  if (!reg->hasHlrData()) {
    reg->insertHlrData();
  }

  if (!reg->hasEirData()) {
    reg->insertEirData();
  }

  reg->logRecords();

  common::SignalHandler::setHandler(
      SIGINT, [this](int signal) { sigintHandler(signal); });
}

void App::run() {
  running = true;

  menu.showStatus();

  ttlManager->start();

  std::jthread connectionHandler{[this]() {
    listener.acceptConnections(SimtelBaseStation::handleConnection);
  }};

  while (running) {
    while (true) {
      auto msg = MessageHolder::instance().takeMsg();
      if (msg) {
        menu.showMessage(*msg);
      } else {
        break;
      }
    }

    if (ttlManager->isActive()) {
      if (ttlManager->isExpired()) {
        break;
      }

      auto warningSec = ttlManager->getWarningSec();
      if (warningSec) {
        menu.showMessage(
            {"TTL: " + std::to_string(*warningSec) + " seconds left",
             common::MenuMessageType::Info});
      }
    }

    std::this_thread::sleep_for(MenuSleepTime);
  }

  exitApp();
}
} // namespace server
