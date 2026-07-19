#pragma once

#include "common/json/info/base_json_info/base_json_info.h"

#include "common/utils/str/str.h"

namespace common {
// класс с информацией для парсинга JSON-объекта
class JsonObjectInfo : public BaseJsonInfo {
private:
  using inner_field_t = std::unique_ptr<BaseJsonInfo>;

  std::unordered_map<std::string, inner_field_t> innerFields{};

public:
  virtual std::optional<std::string> parse(const nlohmann::json &json) override;

  void addInner(const std::string &name, inner_field_t field);
};
} // namespace common
