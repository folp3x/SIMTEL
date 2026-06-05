#pragma once

#include "common/app/menu/command_info/command_info.h"

namespace client {
inline const auto &getCommandsInfo() {
  static const common::CommandInfoMap info = {
      {"exit", {"exit <>", "exit program"}},
      {"active", {"active <1|true|0|false>", "set status"}},
      {"move", {"move <x>", "change location"}},
      {"protocol", {"protocol <b|binary|j|json>", "set data transfer method"}},
      {"sms", {"sms <msisdn> [content]", "send sms"}},
      {"sent", {"sent <>", "show sent sms list"}},
      {"received", {"received <>", "show received sms list"}},
      {"dialog", {"dialog <msisdn|@speed_dial_num>", "show sms converstaion"}}};
  return info;
}
} // namespace client
