#include "error_request.h"

#include "common/network/binary_serializer/binary_serializer.h"
#include "common/network/json_deserializer/json_deserializer.h"

namespace common {
ErrorRequest::ErrorRequest(const std::string &description_)
    : description(description_) {}

std::string ErrorRequest::toStr() const {
  return "Error{description=" + description + "}";
}

RequestType ErrorRequest::getType() const { return RequestType::Error; }

nlohmann::json ErrorRequest::toJson() const {
  return nlohmann::json{{"description", description}};
}

std::optional<std::string>
ErrorRequest::fromJsonStr(const std::string &jsonStr) {
  auto parsedDescription =
      JsonDeserializer::strFromJsonStr(jsonStr, "description");
  if (!parsedDescription) {
    return parsedDescription.error();
  }
  description = *parsedDescription;

  return std::nullopt;
}

std::expected<binary_t, std::string> ErrorRequest::toBinary() const {
  return BinarySerializer::strToBinary(description);
}

std::optional<std::string>
ErrorRequest::fromBinary(const common::binary_t &binary) {
  description = BinarySerializer::strFromBinary(binary);

  return std::nullopt;
}

std::string ErrorRequest::getDescription() const { return description; }
} // namespace common
