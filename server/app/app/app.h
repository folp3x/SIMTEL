#pragma once

#include "common/network/network_address/network_address.h"
#include "server/app/config/bs_config/bs_config/bs_config.h"
#include "server/app/config/config/config.h"
#include "server/app/config/epc_config/epc_config/epc_config.h"
#include "server/app/menu/menu/menu.h"
#include "server/core/simtel/simtel_listener/simtel_listener.h"
#include "server/core/simtel/simtel_mme/simtel_mme.h"

namespace server {
class App {
private:
  static constexpr unsigned int MENU_SLEEP_MS = 100;

  const unsigned int ttlSec;

  SimtelListener listener;

  std::shared_ptr<SimtelRegister> hlr;
  std::shared_ptr<SimtelSmsc> smsc;
  std::vector<std::shared_ptr<SimtelMme>> mmeList;

  std::shared_ptr<TtlManager> ttlManager;

  Menu menu;

  bool isRunning = false;

  void sigintHandler(int signal);

  void exitApp();

public:
  App(const common::NetworkAddress &addr, size_t maxUeThreadsCount,
      const std::vector<MmeConfig> &mmeConfigs, const SmscConfig &smscConfig,
      const std::vector<BsConfig> &bsConfigs, const EpcConfig &epcConfig);

  void run();
};
} // namespace server
