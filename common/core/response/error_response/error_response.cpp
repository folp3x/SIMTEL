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

std::expected<binary_t, std::string> ErrorResponse::toBinary() const {
  common::binary_t binary;
  BinarySerializer::addToBinary(binary, description);
  return binary;
}

std::optional<std::string> ErrorResponse::fromBinary(const binary_t &binary) {
  auto parsedDescription = BinarySerializer::fromBinary<std::string>(binary);
  if (!parsedDescription) {
    return "Description deserialize error";
  }
  description = *parsedDescription;

  return std::nullopt;
}

std::string ErrorResponse::getDescription() const { return description; }
} // namespace common
