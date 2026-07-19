#include "json_object_info.h"

namespace common {
std::optional<std::string> JsonObjectInfo::parse(const nlohmann::json &json) {
  if (!json.is_object()) {
    return "JSON object expected";
  }

  for (const auto &field : innerFields) {
    std::string name = field.first;
    if (!json.contains(name)) {
      return utils::quoted(name) + " field is required";
    }

    auto error = field.second->parse(json[name]);
    if (error) {
      return *error;
    }
  }

  return std::nullopt;
}

void JsonObjectInfo::addInner(const std::string &name,
                              std::unique_ptr<BaseJsonInfo> field) {
  innerFields.emplace(name, std::move(field));
}
} // namespace common
