#pragma once

#include <memory>

#include "menu_item.h"

namespace common {
// абстрактный класс для парсинга команд
class CommandParser {
public:
  virtual ~CommandParser() = default;

  virtual std::unique_ptr<MenuItem>
  parseCommand(const std::string &str) const = 0;
};
} // namespace common
