#pragma once

#include <iostream>
#include <rang.hpp>

namespace common::utils {
inline void printColored(std::string_view output, rang::fg color,
                         std::string_view ending = "\n") {
  std::cout << color << output << rang::fg::reset << ending;
}

inline rang::fg menuMessageTypeToColor(MenuMessageType type) {
  switch (type) {
  case MenuMessageType::Info:
    return rang::fg::cyan;
  case MenuMessageType::Error:
    return rang::fg::red;
  case MenuMessageType::Success:
    return rang::fg::green;
  }
  return rang::fg::reset;
}
} // namespace common::utils
