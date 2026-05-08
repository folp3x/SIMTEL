#pragma once

#include <nlohmann/json.hpp>
#include <optional>

namespace common {
// базовый класс с информацией для парсинга JSON-поля
class ConfigFieldBase {
public:
  virtual ~ConfigFieldBase() = default;

  virtual std::string getName(bool quoted = false) const = 0;
  virtual std::optional<std::string> parse(const nlohmann::json &json) = 0;
};
} // namespace common
