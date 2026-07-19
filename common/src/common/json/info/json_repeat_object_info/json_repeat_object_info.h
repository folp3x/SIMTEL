#pragma once

#include "common/json/info/json_object_info/json_object_info.h"

namespace common {
// класс с информацией для парсинга JSON-массива однотипных объектов
class JsonRepeatObjectInfoInfo : public BaseJsonInfo {
private:
  std::unique_ptr<JsonObjectInfo> innerObject;

  const std::function<void()> objectCallback;

public:
  JsonRepeatObjectInfoInfo(std::unique_ptr<JsonObjectInfo> innerObject_,
                           const std::function<void()> &objectCallback_);

  virtual std::optional<std::string> parse(const nlohmann::json &json) override;
};
} // namespace common
