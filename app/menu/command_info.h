#ifndef COMMAND_INFO_H
#define COMMAND_INFO_H

#include <string>
#include <unordered_map>

struct CommandInfo {
  std::string usage = "";
  std::string description = "";
};

const std::unordered_map<std::string, CommandInfo> &getCommandsInfo();

#endif // COMMAND_INFO_H
