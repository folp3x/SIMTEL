#include "sm_delivery_error_response.h"

#include "common/network/binary_iterator/binary_iterator.h"
#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/network/network.h"

namespace common {
SmDeliveryErrorResponse::SmDeliveryErrorResponse(
    const imsi_t &mTimsi, unsigned int smsId, const std::string &description_)
    : SmDeliveryReportResponse(mTimsi, smsId), description(description_) {}

RequestType SmDeliveryErrorResponse::getType() const {
  return RequestType::SmDeliveryError;
}

nlohmann::json SmDeliveryErrorResponse::toJson() const {
  nlohmann::json json = SmDeliveryReportResponse::toJson();
  json["description"] = description;

  return json;
}

std::optional<std::string>
SmDeliveryErrorResponse::fromJsonStr(const std::string &jsonStr) {
  auto error = SmDeliveryReportResponse::fromJsonStr(jsonStr);
  if (error) {
    return *error;
  }

  auto parsedDescription =
      JsonDeserializer::strFromJsonStr(jsonStr, "description");
  if (!parsedDescription) {
    return parsedDescription.error();
  }
  description = *parsedDescription;

  return std::nullopt;
}

std::expected<binary_t, std::string> SmDeliveryErrorResponse::toBinary() const {
  auto baseBinary = SmDeliveryReportResponse::toBinary();
  if (!baseBinary) {
    return std::unexpected(baseBinary.error());
  }
  binary_t binary = std::move(*baseBinary);

  BinarySerializer::addToBinary(binary, description);

  return binary;
}

std::optional<std::string>
SmDeliveryErrorResponse::fromBinary(const binary_t &binary) {
  auto error = SmDeliveryReportResponse::fromBinary(binary);
  if (error) {
    return *error;
  }

  BinaryIterator it{binary};

  bool skipped = it.skip(SmDeliveryReportResponse::BinaryBytesCount);
  if (!skipped) {
    return "Not enough bytes";
  }

  auto descriptionBinary = it.getRemaining();
  if (!descriptionBinary) {
    return "Binary too short for description";
  }
  auto parsedDescription =
      BinarySerializer::fromBinary<std::string>(*descriptionBinary);
  if (!parsedDescription) {
    return "Description deserialize error";
  }
  description = *parsedDescription;

  return std::nullopt;
}

std::string SmDeliveryErrorResponse::getDescription() const {
  return description;
}
} // namespace common
