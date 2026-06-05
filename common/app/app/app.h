#pragma once

#include "common/app/config/config/config.h"

namespace common {
// абстрактный класс приложения
template <std::derived_from<Config> T> class App {
public:
  virtual ~App() = default;

  virtual void run() = 0;
};
} // namespace common
