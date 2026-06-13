#pragma once

#include "common/json/info/json_base_info/json_base_info.h"
#include "common/json/json_type/json_type.h"

namespace common {
// класс с информацией для парсинга объекта из JSON-поля
class JsonObjectInfo : public JsonBaseInfo {
private:
  const std::string name = "";

  std::vector<std::unique_ptr<JsonBaseInfo>> innerFields = {};

public:
  JsonObjectInfo(const std::string &name,
                 std::vector<std::unique_ptr<JsonBaseInfo>> innerFields_);

  virtual std::optional<std::string> parse(const nlohmann::json &json,
                                           bool finalParse = true) override;

  virtual std::string getName(bool quoted = false) const override;

  virtual nlohmann::json getFieldJson(const nlohmann::json &json) const;
};
} // namespace common
