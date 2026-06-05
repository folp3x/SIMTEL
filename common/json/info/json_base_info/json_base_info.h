#pragma once

#include <nlohmann/json.hpp>
#include <optional>

namespace common {
// абстрактный класс с информацией для парсинга JSON-поля
class JsonBaseInfo {
public:
  virtual ~JsonBaseInfo() = default;

  virtual std::string getName(bool quoted = false) const = 0;
  virtual std::optional<std::string> parse(const nlohmann::json &json,
                                           bool finalParse = true) = 0;
};
} // namespace common
