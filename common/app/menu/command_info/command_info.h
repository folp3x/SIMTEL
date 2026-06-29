#pragma once

#include <string>
#include <unordered_map>

namespace common {
struct CommandInfo {
  std::string usage = "";
  std::string description = "";
};

using CommandInfoMap = std::unordered_map<std::string, CommandInfo>;
} // namespace common
