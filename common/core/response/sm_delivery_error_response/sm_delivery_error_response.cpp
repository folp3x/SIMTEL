#include "sm_delivery_error_response.h"

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

std::vector<std::unique_ptr<BaseBinaryInfo>>
SmDeliveryErrorResponse::getBinaryValuesInfo() {
  std::vector<std::unique_ptr<BaseBinaryInfo>> valuesInfo =
      SmDeliveryReportResponse::getBinaryValuesInfo();

  valuesInfo.emplace_back(makeBinaryValue(&description));

  return valuesInfo;
}

std::string SmDeliveryErrorResponse::getDescription() const {
  return description;
}
} // namespace common
