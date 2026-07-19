#include "error_response.h"

namespace common {
ErrorResponse::ErrorResponse(const std::string &description_)
    : description(description_) {}

RequestType ErrorResponse::getType() const { return RequestType::Error; }

nlohmann::json ErrorResponse::toJson() const {
  return nlohmann::json{{"description", description}};
}

std::unique_ptr<BaseJsonInfo> ErrorResponse::getJsonRootInfo() {
  auto root = makeJsonObject();
  root->addInner("description", makeJsonValue(&description));
  return root;
}

std::vector<std::unique_ptr<BaseBinaryInfo>>
ErrorResponse::getBinaryValuesInfo() {
  std::vector<std::unique_ptr<BaseBinaryInfo>> valuesInfo{};
  valuesInfo.emplace_back(makeBinaryValue(&description));
  return valuesInfo;
}

std::string ErrorResponse::getDescription() const { return description; }
} // namespace common
