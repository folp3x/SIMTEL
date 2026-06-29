#pragma once

#include <nlohmann/json.hpp>
#include <optional>

namespace common {
// абстрактный класс с информацией для парсинга JSON-поля
class JsonBaseInfo {
protected:
  const std::string name = "";

  std::string getName(bool quoted = false) const;

  nlohmann::json getFieldJson(const nlohmann::json &json) const;

public:
  explicit JsonBaseInfo(const std::string &name_);

  virtual ~JsonBaseInfo() = default;

  virtual std::optional<std::string> parse(const nlohmann::json &json,
                                           bool finalParse = true) = 0;
};
} // namespace common
