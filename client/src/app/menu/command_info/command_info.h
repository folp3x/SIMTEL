#pragma once

#include "common/app/menu/command_info/command_info.h"

namespace client {
inline const auto &getCommandsInfo() {
  static const std::unordered_map<std::string, common::CommandInfo> info = {
      {"exit", {"exit <>", _("exit program")}},
      {"active", {"active <1|true|0|false>", _("set status")}},
      {"move", {"move <x>", _("change location")}},
      {"protocol",
       {"protocol <b|binary|j|json>", _("set data transfer method")}},
      {"sms", {"sms <msisdn|@speed_dial> [content]", _("send sms")}},
      {"sent", {"sent <>", _("show all sent sms")}},
      {"received", {"received <>", _("show all received sms")}},
      {"dialog", {"dialog <msisdn|@speed_dial>", _("show sms converstaion")}},
      {"*#", {"*<code>#", _("send ussd command")}},
      {"ussd", {"ussd <>", _("show ussd codes")}}};
  return info;
}
} // namespace client
