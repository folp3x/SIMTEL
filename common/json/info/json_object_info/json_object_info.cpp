#include "json_object_info.h"

namespace common {
JsonObjectInfo::JsonObjectInfo(
    const std::string &name,
    std::vector<std::unique_ptr<JsonBaseInfo>> innerFields_)
    : innerFields(std::move(innerFields_)) {}

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
    auto error = field->parse(fieldJson);
    if (error) {
      return error;
    }
  }

  return std::nullopt;
}

std::string JsonObjectInfo::getName(bool quoted) const {
  return quoted ? "'" + name + "'" : name;
}

nlohmann::json JsonObjectInfo::getFieldJson(const nlohmann::json &json) const {
  return name.empty() ? json : json[name];
}
} // namespace common
