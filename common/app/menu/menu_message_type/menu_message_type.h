#pragma once

#include <rang.hpp>

namespace common {
enum class MenuMessageType : uint8_t { Info, Error, Success, NoColor };

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
} // namespace common
