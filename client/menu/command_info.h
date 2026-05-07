#pragma once

#include <string>
#include <unordered_map>

#include "common/app/menu/command_info.h"

namespace client {
const std::unordered_map<std::string, common::CommandInfo> &getCommandsInfo();
}
