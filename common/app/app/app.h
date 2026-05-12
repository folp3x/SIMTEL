#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include <memory>
#include <spdlog/spdlog.h>

#include "common/app/config/config/config.h"
#include "common/app/menu/menu_item/menu_item/menu_item.h"
#include "common/core/location/location.h"
#include "common/network/network_address/network_address.h"
#include "common/network/protocol/protocol.h"
#include "common/utils/str/str.h"

namespace common {
// абстрактный класс приложения
template <std::derived_from<Config> T> class App {
private:
  void logConstructor(const std::string constructorType,
                      const Location<float> &location,
                      const NetworkAddress &addr) const {
    SPDLOG_LOGGER_DEBUG(
        spdlog::default_logger(),
        "common::App {} constructor called: location={}, addr={}",
        constructorType, location.toStr(), addr.toStr());
  }

protected:
  Location<float> location{{0, 0, 0}};
  Protocol protocol = Protocol::JSON;

  const NetworkAddress addr{"127.0.0.1:49152"};

  void logCommandProcess(std::string_view commandName,
                         std::string_view argsStr = "") const {
    std::string nameUpper = uppercased(commandName);
    if (!argsStr.empty()) {
      SPDLOG_LOGGER_INFO(spdlog::default_logger(),
                         "Processing command {} with args: {}", nameUpper,
                         argsStr);
    } else {
      SPDLOG_LOGGER_INFO(spdlog::default_logger(), "Processing command {}",
                         nameUpper);
    }
  }

public:
  virtual ~App() = default;

  App(const Location<float> &location_, const NetworkAddress &addr_)
      : location(location_), addr(addr_) {}

  App(const App &other) : location(other.location), addr(other.addr) {
    logConstructor("COPY", location, addr);
  };

  App(App &&other) noexcept
      : location(std::move(other.location)), addr(std::move(other.addr)) {
    logConstructor("MOVE", location, addr);
  };

  virtual std::string handleCommand(const std::unique_ptr<MenuItem> &cmd,
                                    bool &exit) = 0;

  virtual void run() = 0;
};
} // namespace common
