#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

#include "common/app/menu/menu_item/menu_item/menu_item.h"

namespace common {
class CommandParser {
protected:
  using args_t = std::vector<std::string>;

  // парсер, принимающий изначальную строку, вектор аргументов и out-параметр
  // для доп. сообщения
  using arg_parser_t = std::function<std::unique_ptr<MenuItem>(
      const std::string &, const args_t &, std::string &)>;

  using arg_parser_map_t = std::unordered_map<std::string, arg_parser_t>;

  virtual arg_parser_map_t getArgsParsers() const = 0;

public:
  virtual ~CommandParser() = default;

  virtual std::unique_ptr<MenuItem> parseCommand(const std::string &str,
                                                 std::string &extraMsg) const;
};
} // namespace common
