#include "empty_request.h"

namespace common {
std::string EmptyRequest::toStr() const { return "Empty"; }

RequestType EmptyRequest::getType() const { return RequestType::Unknown; }

nlohmann::json EmptyRequest::toJson() const { return nlohmann::json{}; }

std::optional<std::string>
EmptyRequest::fromJsonStr(const std::string &jsonStr) {
  return std::nullopt;
}

std::expected<binary_t, std::string> EmptyRequest::toBinary() const {
  return binary_t{};
}

std::optional<std::string>
EmptyRequest::fromBinary(const common::binary_t &binary) {
  return std::nullopt;
}
} // namespace common
