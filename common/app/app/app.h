#pragma once

#include <memory>

#include "common/app/config/config/config.h"
#include "common/app/menu/menu_item/menu_item/menu_item.h"
#include "common/app/menu/menu_message/menu_message.h"
#include "common/core/location/location/location.h"

namespace common {
// абстрактный класс приложения
template <std::derived_from<Config> T> class App {
private:
  void logConstructor(const std::string &constructorType,
                      const Location<> &location) const;

protected:
  using Messages = std::vector<common::MenuMessage>;

  Location<> location{};

  void logCommandProcess(std::string_view commandName,
                         std::string_view argsStr = "") const;

public:
  virtual ~App() = default;

  App(const Location<> &location_);
  App(const App &other);
  App(App &&other) noexcept;

  virtual Messages handleCommand(const std::unique_ptr<MenuItem> &cmd,
                                 bool &exit) = 0;

  virtual void run() = 0;
};
} // namespace common

#include "app_impl.h"
