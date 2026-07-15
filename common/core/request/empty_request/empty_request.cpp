#include "empty_request.h"

namespace common {
RequestType EmptyRequest::getType() const { return RequestType::Empty; }

nlohmann::json EmptyRequest::toJson() const { return nlohmann::json{}; }

std::optional<std::string>
EmptyRequest::fromJsonStr(const std::string &jsonStr) {
  return std::nullopt;
}

std::vector<std::unique_ptr<BaseBinaryInfo>>
EmptyRequest::getBinaryValuesInfo() {
  return {};
}
} // namespace common
