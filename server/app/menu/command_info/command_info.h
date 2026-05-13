#pragma once

#include <string>
#include <unordered_map>

#include "common/app/menu/command_info/command_info.h"

namespace server {
// возвращает map вида 'название команды: {использование, описание}'
inline const common::CommandInfoMap &getCommandsInfo() {
  static const common::CommandInfoMap info = {
      {"exit", {"exit <>", "exit program"}},
      {"dist", {"dist <x> <y> <z>", "calculate distance to location"}}};
  return info;
}
} // namespace server
