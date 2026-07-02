#include "json_object_info.h"

#include "common/json/json_type/json_type.h"

namespace common {
JsonObjectInfo::JsonObjectInfo(const std::string &name) : JsonBaseInfo(name) {}

std::optional<std::string> JsonObjectInfo::parse(const nlohmann::json &json,
                                                 bool finalParse) {
  std::string showedName = name.empty() ? "root element" : getName(true);
  if (!name.empty() && !json.contains(name)) {
    return showedName + " is required";
  }

  nlohmann::json fieldJson = getFieldJson(json);
  if (!hasJsonType(fieldJson, nlohmann::json::value_t::object)) {
    return showedName + " must have a type 'object'";
  }

  for (const auto &field : innerFields) {
    auto error = field->parse(fieldJson);
    if (error) {
      return error;
    }
  }

  return std::nullopt;
}

void JsonObjectInfo::addInner(std::unique_ptr<JsonBaseInfo> field) {
  innerFields.push_back(std::move(field));
}
} // namespace common
