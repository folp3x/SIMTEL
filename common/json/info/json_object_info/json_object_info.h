#pragma once

#include "common/json/info/json_base_info/json_base_info.h"

namespace common {
// класс с информацией для парсинга JSON-объекта
class JsonObjectInfo : public JsonBaseInfo {
private:
  std::vector<std::unique_ptr<JsonBaseInfo>> innerFields = {};

public:
  explicit JsonObjectInfo(const std::string &name);

  virtual std::optional<std::string> parse(const nlohmann::json &json,
                                           bool finalParse = true) override;

  void addInner(std::unique_ptr<JsonBaseInfo> field);
};
} // namespace common
