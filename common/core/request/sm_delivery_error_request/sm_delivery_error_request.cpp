#include "sm_delivery_error_request.h"

#include "common/network/binary_iterator/binary_iterator.h"
#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/network/network.h"

namespace common {
SmDeliveryErrorRequest::SmDeliveryErrorRequest(const imsi_t &mTimsi,
                                               unsigned int smsId,
                                               const std::string &description_)
    : SmDeliveryReportRequest(mTimsi, smsId), description(description_) {}

RequestType SmDeliveryErrorRequest::getType() const {
  return RequestType::SM_Delivery_Error;
}

nlohmann::json SmDeliveryErrorRequest::toJson() const {
  nlohmann::json json = SmDeliveryReportRequest::toJson();
  json["description"] = description;

  return json;
}

std::optional<std::string>
SmDeliveryErrorRequest::fromJsonStr(const std::string &jsonStr) {
  auto error = SmDeliveryReportRequest::fromJsonStr(jsonStr);
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

std::expected<binary_t, std::string> SmDeliveryErrorRequest::toBinary() const {
  auto binary = SmDeliveryReportRequest::toBinary();
  if (!binary) {
    return std::unexpected(binary.error());
  }

  binary_t binDescription = BinarySerializer::strToBinary(description);

  return mergeBinary(*binary, binDescription);
}

std::optional<std::string>
SmDeliveryErrorRequest::fromBinary(const binary_t &binary) {
  auto error = SmDeliveryReportRequest::fromBinary(binary);
  if (error) {
    return *error;
  }

  BinaryIterator it{binary};

  bool skipped = it.skip(SmDeliveryReportRequest::binaryBytesCount);
  if (!skipped) {
    return "Not enough bytes";
  }

  auto descriptionBinary = it.getRemaining();
  if (!descriptionBinary) {
    return "Binary too short for description";
  }
  description = BinarySerializer::strFromBinary(*descriptionBinary);

  return std::nullopt;
}

std::string SmDeliveryErrorRequest::getDescription() const {
  return description;
}
} // namespace common
