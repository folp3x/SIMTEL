#include "error_response.h"

#include "common/network/json_deserializer/json_deserializer.h"

namespace common {
ErrorResponse::ErrorResponse(const std::string &description_)
    : description(description_) {}

RequestType ErrorResponse::getType() const { return RequestType::Error; }

nlohmann::json ErrorResponse::toJson() const {
  return nlohmann::json{{"description", description}};
}

std::optional<std::string>
ErrorResponse::fromJsonStr(const std::string &jsonStr) {
  auto parsedDescription =
      JsonDeserializer::strFromJsonStr(jsonStr, "description");
  if (!parsedDescription) {
    return parsedDescription.error();
  }
  description = *parsedDescription;

  return std::nullopt;
}

std::vector<std::unique_ptr<BaseBinaryInfo>>
ErrorResponse::getBinaryValuesInfo() {
  std::vector<std::unique_ptr<BaseBinaryInfo>> valuesInfo{};
  valuesInfo.emplace_back(makeBinaryValue(&description));
  return valuesInfo;
}

std::string ErrorResponse::getDescription() const { return description; }
} // namespace common
