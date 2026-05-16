#pragma once

#include "common/app/app/app.h"

#include "common/app/menu/menu_item/menu_item/menu_item.h"
#include "common/network/network_address/network_address.h"
#include "server/app/config/config/config.h"
#include "server/network/socket/socket.h"

namespace server {
class App : common::App<Config> {
private:
  std::unique_ptr<Socket> sock;
  common::NetworkAddress addr;

  virtual Messages handleCommand(const std::unique_ptr<common::MenuItem> &cmd,
                                 bool &exit) override;

public:
  App(const common::Location<> &location_, const common::NetworkAddress &addr_);

  virtual void run() override;
};
} // namespace server
