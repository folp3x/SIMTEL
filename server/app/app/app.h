#pragma once

#include "common/network/network_address/network_address.h"

#include "server/app/config/bs_config/bs_config/bs_config.h"
#include "server/app/config/config/config.h"
#include "server/app/config/epc_config/epc_config/epc_config.h"

#include "server/app/menu/menu/menu.h"

#include "server/core/simtel/pcrf/simtel_pcrf/simtel_pcrf.h"
#include "server/core/simtel/simtel_listener/simtel_listener.h"
#include "server/core/simtel/simtel_mme/simtel_mme.h"

namespace server {
class App {
private:
  static constexpr std::chrono::milliseconds MenuSleepTime{1000};
  static constexpr unsigned int TtlWarningPeriodSec = 1;

  SimtelListener listener;

  std::shared_ptr<SimtelRegister> reg;
  std::shared_ptr<SimtelSmsc> smsc;
  std::unordered_map<unsigned int, std::shared_ptr<SimtelMme>> mmeList{};

  std::shared_ptr<SimtelPcrf> pcrf;

  std::shared_ptr<TtlManager> ttlManager;

  Menu menu;

  bool running = false;

  void sigintHandler(int signal);

  void exitApp();

public:
  App(const common::NetworkAddress &addr, size_t maxUeThreadsCount,
      const std::vector<MmeConfig> &mmeConfigs, const SmscConfig &smscConfig,
      const std::vector<BsConfig> &bsConfigs, const EpcConfig &epcConfig,
      const PcrfConfig &pcrfConfig);

  void run();
};
} // namespace server
