#pragma once

#include "common/app/app/app.h"

#include <atomic>
#include <memory>
#include <queue>

#include "common/app/menu/menu_item/menu_item/menu_item.h"
#include "common/network/network_address/network_address.h"
#include "server/app/config/config/config.h"
#include "server/network/socket/socket.h"

namespace server {
class App : common::App<Config> {
private:
  bool isRunning = true;

  std::unique_ptr<Socket> sock;
  common::NetworkAddress addr;

  std::atomic<int> activeClientThreads{0};

  std::priority_queue<common::MenuMessage> messages{};

  virtual void handleCommand(const std::unique_ptr<common::MenuItem> &cmd,
                             bool &exit) override;

  void handleSingleClient(const std::unique_ptr<Socket> &clientSock);
  void handleClients();

public:
  App(const common::Location<> &location_, const common::NetworkAddress &addr_);

  virtual void run() override;
};
} // namespace server
