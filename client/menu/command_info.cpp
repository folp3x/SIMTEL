#include "command_info.h"

namespace client {
// возвращает map вида 'название команды: {использование, описание}'
const std::unordered_map<std::string, common::CommandInfo> &getCommandsInfo() {
  static const std::unordered_map<std::string, common::CommandInfo> info = {
      {"exit", {"exit <>", "exit programm"}},
      {"active", {"active <1|true|0|false>", "set status"}},
      {"move", {"move <x> [y] [z]", "change location (y and z are optional)"}},
      {"protocol", {"protocol <binary|json>", "set data transfer method"}}};
  return info;
}
} // namespace client
