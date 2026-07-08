#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

#include "common/app/menu/menu_item/menu_item/menu_item.h"

namespace common {
class CommandParser {
protected:
  using Args = std::vector<std::string>;

  // парсер, принимающий изначальную строку, вектор аргументов и out-параметр
  // для доп. сообщения
  using ArgParser = std::function<std::unique_ptr<MenuItem>(
      const std::string &, const Args &, std::string &)>;

  using ArgsParsersMap = std::unordered_map<std::string, ArgParser>;

  virtual ArgsParsersMap getArgsParsers() const = 0;

public:
  virtual ~CommandParser() = default;

  virtual std::unique_ptr<MenuItem> parseCommand(const std::string &str,
                                                 std::string &extraMsg) const;
};
} // namespace common
