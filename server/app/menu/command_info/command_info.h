#pragma once

#include "common/app/menu/command_info/command_info.h"

namespace server {
inline const auto &getCommandsInfo() {
  static const common::CommandInfoMap info = {
      {"exit", {"exit <>", "exit program"}},
      {"dist", {"dist <x> <y> <z>", "calculate distance to location"}}};
  return info;
}
} // namespace server
