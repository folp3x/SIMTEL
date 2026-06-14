#include "json_object_info.h"

#include "common/json/json_type/json_type.h"

#include <iostream>

namespace common {
JsonObjectInfo::JsonObjectInfo(const std::string &name) : JsonBaseInfo(name) {}

std::optional<std::string> JsonObjectInfo::parse(const nlohmann::json &json,
                                                 bool finalParse) {
  std::string nameQuoted = getName(true);
  if (!name.empty() && !json.contains(name)) {
    return nameQuoted + " is required";
  }

  const auto &fieldJson = getFieldJson(json);
  if (!hasJsonType(fieldJson, nlohmann::json::value_t::object)) {
    return nameQuoted + " must have a type 'object'";
  }

  for (const auto &field : innerFields) {
    std::cout << "Object " << nameQuoted << std::endl;
    std::cout << fieldJson.dump() << std::endl;
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
