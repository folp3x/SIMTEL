#include "json_parsable.h"

namespace common {
std::unique_ptr<JsonObjectInfo> JsonParsable::makeJsonObject() {
  return std::make_unique<JsonObjectInfo>();
}

std::unique_ptr<JsonRepeatObjectInfoInfo> JsonParsable::makeJsonRepeatObject(
    std::unique_ptr<JsonObjectInfo> objectInfo,
    const std::function<void()> &objectCallback) {
  return std::make_unique<JsonRepeatObjectInfoInfo>(std::move(objectInfo),
                                                    objectCallback);
}

std::optional<std::string> JsonParsable::fromJsonStr(const std::string str) {
  try {
    nlohmann::json json = nlohmann::json::parse(str);

    return fromJson(json);
  } catch (const nlohmann::json::exception &e) {
    return "JSON parse error: " + std::string(e.what());
  }
}

std::optional<std::string> JsonParsable::fromJson(const nlohmann::json &json) {
  return getJsonRootInfo()->parse(json);
}

std::optional<std::string>
JsonParsable::fromJsonFile(const std::string &filePath) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    return "Failed to open file";
  }

  try {
    nlohmann::json json;

    file >> json;

    if (file.fail()) {
      return "Failed to read file content";
    }

    return fromJson(json);
  } catch (const nlohmann::json::exception &e) {
    return "JSON parse error: " + std::string(e.what());
  }
}
} // namespace common
