#include "json_base_info.h"

namespace common {
std::string JsonBaseInfo::getName(bool quoted) const {
  return quoted ? "'" + name + "'" : name;
}

nlohmann::json JsonBaseInfo::getFieldJson(const nlohmann::json &json) const {
  return name.empty() ? json : json[name];
}

JsonBaseInfo::JsonBaseInfo(const std::string &name_) : name(name_) {}
} // namespace common
