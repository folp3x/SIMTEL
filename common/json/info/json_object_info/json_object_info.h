#pragma once

#include "common/json/info/base_json_info/base_json_info.h"

namespace common {
// класс с информацией для парсинга JSON-объекта
class JsonObjectInfo : public BaseJsonInfo {
private:
  std::unordered_map<std::string, std::unique_ptr<BaseJsonInfo>> innerFields{};

public:
  virtual std::optional<std::string> parse(const nlohmann::json &json) override;

  void addInner(const std::string &name, std::unique_ptr<BaseJsonInfo> field);
};
} // namespace common
