#pragma once

#include <string>
#include <unordered_map>

#include "common/app/menu/command_info/command_info.h"

namespace client {
// возвращает map вида 'название команды: {использование, описание}'
inline const common::CommandInfoMap &getCommandsInfo() {
  static const common::CommandInfoMap info = {
      {"exit", {"exit <>", "exit programm"}},
      {"active", {"active <1|true|0|false>", "set status"}},
      {"move", {"move <x> [y] [z]", "change location (y and z are optional)"}},
      {"protocol", {"protocol <binary|json>", "set data transfer method"}}};
  return info;
}
} // namespace client
