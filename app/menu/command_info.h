#pragma once

#include <string>
#include <unordered_map>

struct CommandInfo {
  std::string usage = "";
  std::string description = "";
};

const std::unordered_map<std::string, CommandInfo> &getCommandsInfo();
