#pragma once

#include "common/app/app/app.h"

#include "common/network/network_address/network_address.h"
#include "server/app/config/bs_config/bs_config/bs_config.h"
#include "server/app/config/config/config.h"
#include "server/app/config/epc_config/epc_config/epc_config.h"
#include "server/app/menu/menu/menu.h"
#include "server/app/message_holder/message_holder.h"
#include "server/core/simtel/simtel_listener/simtel_listener.h"

namespace server {
class App : common::App<Config> {
private:
  SimtelListener listener;

  Menu menu;

  bool isRunning = false;

  static constexpr unsigned int MENU_SLEEP_MS = 100;

  void sigintHandler(int signal);

public:
  App(const common::NetworkAddress &addr, size_t maxUeThreadsCount,
      const std::vector<MmeConfig> &mmeConfigs, const SmscConfig &smscConfig,
      const std::vector<BsConfig> &bsConfigs, const EpcConfig &epcConfig);

  virtual void run() override;
};
} // namespace server
