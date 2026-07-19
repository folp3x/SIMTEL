#include "empty_request.h"

namespace common {
RequestType EmptyRequest::getType() const { return RequestType::Empty; }

nlohmann::json EmptyRequest::toJson() const { return nlohmann::json::object(); }

std::unique_ptr<BaseJsonInfo> EmptyRequest::getJsonRootInfo() {
  return makeJsonObject();
}

std::vector<std::unique_ptr<BaseBinaryInfo>>
EmptyRequest::getBinaryValuesInfo() {
  return {};
}
} // namespace common
