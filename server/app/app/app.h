#pragma once

#include "common/app/app/app.h"

#include <atomic>
#include <memory>
#include <queue>

#include "common/app/menu/menu_item/menu_item/menu_item.h"
#include "common/app/menu/menu_message/menu_message.h"
#include "common/core/location/location/location.h"
#include "common/network/network_address/network_address.h"
#include "server/app/config/config/config.h"
#include "server/core/simtel/simtel_listener/simtel_listener.h"
#include "server/network/socket/socket.h"

namespace server {
class App : common::App<Config> {
private:
  SimtelListener listener;

  common::Location<> location{};

  std::mutex messagesMtx{};
  std::priority_queue<common::MenuMessage> messages{};

  void sigintHandler(int signal);

  void handleSingleClient(const std::unique_ptr<Socket> &clientSock);

public:
  App(const common::Location<> &location_, const common::NetworkAddress &addr);

  virtual void run() override;
};
} // namespace server
