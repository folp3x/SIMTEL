#include "json_object_array_info.h"

#include "common/json/json_type/json_type.h"

namespace common {
JsonObjectArrayInfo::JsonObjectArrayInfo(
    const std::string &name, std::unique_ptr<JsonObjectInfo> innerObject_,
    const std::function<void()> &objectCallback_)
    : JsonBaseInfo(name), innerObject(std::move(innerObject_)),
      objectCallback(objectCallback_) {}

std::optional<std::string>
JsonObjectArrayInfo::parse(const nlohmann::json &json, bool finalParse) {
  std::string showedName = name.empty() ? "root element" : getName(true);
  if (!name.empty() && !json.contains(name)) {
    return showedName + " is required";
  }

  nlohmann::json fieldJson = getFieldJson(json);
  if (!hasJsonType(fieldJson, nlohmann::json::value_t::array)) {
    return showedName + " must have a type 'array'";
  }

  for (size_t i = 0; i < fieldJson.size(); ++i) {
    nlohmann::json elemJson = fieldJson[i];
    auto error = innerObject->parse(elemJson);
    if (error) {
      return error;
    }

    objectCallback();
  }

  return std::nullopt;
}
} // namespace common
