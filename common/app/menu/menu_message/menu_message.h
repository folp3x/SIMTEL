#pragma once

#include <string>

#include "common/app/menu/menu_message_type/menu_message_type.h"

namespace common {
struct MenuMessage {
  std::string content = "";
  MenuMessageType type = MenuMessageType::INFO;

  bool operator<(const MenuMessage &other) const { return type < other.type; }
};
} // namespace common
