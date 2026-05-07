#pragma once

#include <memory>

#include "common/app/config/config.h"
#include "common/app/menu/menu_item.h"
#include "common/core/location.h"
#include "common/core/network_address.h"
#include "common/core/protocol.h"

namespace common {
// абстрактный класс приложения
template <std::derived_from<Config> T> class App {
protected:
  Location location{{0, 0, 0}};
  Protocol protocol = Protocol::JSON;

  NetworkAddress addr{"127.0.0.1:49152"};

public:
  virtual ~App() = default;

  virtual std::string
  handleCommand(const std::unique_ptr<common::MenuItem> &cmd, bool &exit) = 0;

  virtual void run() = 0;
};
} // namespace common
