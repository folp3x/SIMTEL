#pragma once

#include <iostream>
#include <rang.hpp>
#include <string>

namespace common {
inline void printColored(const std::string_view &output, rang::fg color,
                         const std::string_view &ending = "\n") {
  std::cout << color << output << rang::fg::reset << ending;
}
} // namespace common
