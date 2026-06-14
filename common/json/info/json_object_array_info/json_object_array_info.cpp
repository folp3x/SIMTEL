#include "json_object_array_info.h"

#include "common/json/json_type/json_type.h"

namespace common {
JsonObjectArrayInfo::JsonObjectArrayInfo(
    const std::string &name, JsonObjectInfo innerObject_,
    const std::function<void()> &objectCallback_)
    : JsonBaseInfo(name), innerObject(std::move(innerObject_)),
      objectCallback(objectCallback_) {}

std::optional<std::string>
JsonObjectArrayInfo::parse(const nlohmann::json &json, bool finalParse) {
  std::string nameQuoted = getName(true);
  if (!name.empty() && !json.contains(name)) {
    return nameQuoted + " is required";
  }

  const auto &fieldJson = getFieldJson(json);
  if (!hasJsonType(fieldJson, nlohmann::json::value_t::array)) {
    return nameQuoted + " must have a type 'array'";
  }

  for (size_t i = 0; i < fieldJson.size(); ++i) {
    auto elemJson = fieldJson[i];
    auto error = innerObject.parse(elemJson);
    if (error) {
      return error;
    }

    objectCallback();
  }

  return std::nullopt;
}
} // namespace common
