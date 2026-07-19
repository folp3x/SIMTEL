#include "json_repeat_object_info.h"

namespace common {
JsonRepeatObjectInfoInfo::JsonRepeatObjectInfoInfo(
    std::unique_ptr<JsonObjectInfo> innerObject_,
    const std::function<void()> &objectCallback_)
    : innerObject(std::move(innerObject_)), objectCallback(objectCallback_) {}

std::optional<std::string>
JsonRepeatObjectInfoInfo::parse(const nlohmann::json &json) {
  if (!json.is_array()) {
    return "JSON array expected";
  }

  for (size_t i = 0; i < json.size(); ++i) {
    nlohmann::json objectJson = json[i];
    auto error = innerObject->parse(objectJson);
    if (error) {
      return *error;
    }

    objectCallback();
  }

  return std::nullopt;
}
} // namespace common
