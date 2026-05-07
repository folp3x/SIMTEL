#pragma once

#include <nlohmann/json.hpp>

namespace common {
// базовый класс с информацией для парсинга JSON-поля
class ConfigFieldBase {
public:
  virtual ~ConfigFieldBase() = default;

  virtual std::string getName(bool quoted = false) const = 0;
  virtual bool parse(const nlohmann::json &json, std::string &msg) = 0;
};
} // namespace common
