#pragma once

#include "common/app/app/app.h"

#include "common/network/network_address/network_address.h"
#include "server/app/config/config/config.h"
#include "server/app/menu/menu/menu.h"
#include "server/app/message_holder/message_holder.h"
#include "server/core/simtel/simtel_listener/simtel_listener.h"

namespace server {
class App : common::App<Config> {
private:
  SimtelListener listener;

  Menu menu;

  std::shared_ptr<MessageHolder> msgHolder{};

  void sigintHandler(int signal);

public:
  App(const common::NetworkAddress &addr, size_t maxUeThreads);

  virtual void run() override;
};
} // namespace server
