#include "sm_delivery_report_response.h"

#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/str/str.h"
#include "common/validator/validator.h"

namespace common {
SmDeliveryReportResponse::SmDeliveryReportResponse(const imsi_t &mTimsi_,
                                                   unsigned int smsId_)
    : mTimsi(mTimsi_), smsId(smsId_) {}

RequestType SmDeliveryReportResponse::getType() const {
  return RequestType::SmDeliveryReport;
}

nlohmann::json SmDeliveryReportResponse::toJson() const {
  return nlohmann::json{{"mTimsi", mTimsi}, {"smsId", smsId}};
}

std::optional<std::string>
SmDeliveryReportResponse::fromJsonStr(const std::string &jsonStr) {
  auto parsedMTimsi = JsonDeserializer::imsiFromJsonStr(jsonStr, "mTimsi");
  if (!parsedMTimsi) {
    return parsedMTimsi.error();
  }
  mTimsi = *parsedMTimsi;

  auto parsedSmsId = JsonDeserializer::smsIdFromJsonStr(jsonStr, "smsId");
  if (!parsedSmsId) {
    return parsedSmsId.error();
  }
  smsId = *parsedSmsId;

  return std::nullopt;
}

std::vector<std::unique_ptr<BaseBinaryInfo>>
SmDeliveryReportResponse::getBinaryValuesInfo() {
  std::vector<std::unique_ptr<BaseBinaryInfo>> valuesInfo{};

  valuesInfo.emplace_back(makeBinaryValue<imsi_t, uint64_t>(
      &mTimsi, Validator::isCorrectImsi, utils::identifierFromStr,
      utils::imsiToStr));

  valuesInfo.emplace_back(
      makeBinaryValue<unsigned int>(&smsId, Validator::isCorrectSmsId));

  return valuesInfo;
}

imsi_t SmDeliveryReportResponse::getMTimsi() const { return mTimsi; }

unsigned int SmDeliveryReportResponse::getSmsId() const { return smsId; }
} // namespace common
