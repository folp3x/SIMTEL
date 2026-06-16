#pragma once

#include <rang.hpp>

namespace common {
enum class MenuMessageType : uint8_t { INFO, ERR, NO_COLOR, SUCCESS };

inline rang::fg menuMessageTypeToColor(MenuMessageType type) {
  switch (type) {
  case MenuMessageType::INFO:
    return rang::fg::cyan;
  case MenuMessageType::ERR:
    return rang::fg::red;
  case MenuMessageType::SUCCESS:
    return rang::fg::green;
  }
  return rang::fg::reset;
}
} // namespace common
