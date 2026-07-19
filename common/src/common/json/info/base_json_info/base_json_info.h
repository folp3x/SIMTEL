#pragma once

#include <nlohmann/json.hpp>
#include <optional>

namespace common {
// абстрактный класс с информацией для парсинга JSON-поля
class BaseJsonInfo {
public:
  virtual ~BaseJsonInfo() = default;

  virtual std::optional<std::string> parse(const nlohmann::json &json) = 0;
};
} // namespace common
