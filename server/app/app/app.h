#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include "common/app/app/app.h"

#include "common/app/menu/menu_item/menu_item/menu_item.h"
#include "server/app/config/config/config.h"

namespace server {
class App : common::App<Config> {
private:
  void logConstructor(const std::string constructorType,
                      const common::Location<float> &location,
                      const common::NetworkAddress &addr) const;

  virtual std::string
  handleCommand(const std::unique_ptr<common::MenuItem> &cmd,
                bool &exit) override;

public:
  App(const common::Location<float> &location_, const common::NetworkAddress &addr_);

  App(const App &other);
  App(App &&other) noexcept;

  virtual void run() override;
};
} // namespace server
