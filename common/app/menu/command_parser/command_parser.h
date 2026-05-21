#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

#include "common/app/menu/command_info/command_info.h"
#include "common/app/menu/menu_item/menu_item/menu_item.h"

namespace common {
class CommandParser {
protected:
  // парсер принимающий вектор аргументов и out-парамер для доп. сообщения
  using ArgsParsersMap =
      std::unordered_map<std::string,
                         std::function<std::unique_ptr<MenuItem>(
                             const std::vector<std::string> &, std::string &)>>;

  virtual ArgsParsersMap getArgsParsers() const = 0;

  static std::unique_ptr<MenuItem>
  parseExitArgs(const std::vector<std::string> &args, std::string &extraMsg);

public:
  std::unique_ptr<MenuItem> parseCommand(const std::string &str,
                                         std::string &extraMsg) const;
};
} // namespace common
