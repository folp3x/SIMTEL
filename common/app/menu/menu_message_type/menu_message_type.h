#pragma once

#include <rang.hpp>

namespace common {
enum class MenuMessageType : uint8_t { INFO, ERR, NOCOLOR };

inline rang::fg menuMessageTypeToColor(MenuMessageType type) {
  switch (type) {
  case MenuMessageType::INFO:
    return rang::fg::cyan;
  case MenuMessageType::ERR:
    return rang::fg::red;
  }
  return rang::fg::reset;
}
} // namespace common
