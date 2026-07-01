#pragma once

#include <string>

namespace common {
class MenuItem {
public:
  virtual ~MenuItem() = default;

  virtual std::string_view getName() const = 0;
};
} // namespace common
