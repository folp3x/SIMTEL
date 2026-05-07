#pragma once

#include <string>

namespace common {
// абстрактный класс команды
class MenuItem {
public:
  virtual ~MenuItem() = default;

  virtual std::string getName() const = 0;
};
} // namespace common
