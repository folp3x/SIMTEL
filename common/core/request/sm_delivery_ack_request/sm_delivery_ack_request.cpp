#include "sm_delivery_ack_request.h"

#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/str/str.h"
#include "common/validator/validator.h"

namespace common {
SmDeliveryAckRequest::SmDeliveryAckRequest(const imsi_t &mTimsi_,
                                           unsigned int smsId_,
                                           const msisdn_t &msisdn_)
    : mTimsi(mTimsi_), smsId(smsId_), msisdn(msisdn_) {}

RequestType SmDeliveryAckRequest::getType() const {
  return RequestType::SmDeliveryAck;
}

nlohmann::json SmDeliveryAckRequest::toJson() const {
  return nlohmann::json{
      {"mTimsi", mTimsi}, {"smsId", smsId}, {"msisdn", msisdn}};
}

std::optional<std::string>
SmDeliveryAckRequest::fromJsonStr(const std::string &jsonStr) {
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

  auto parsedMsisdn = JsonDeserializer::strFromJsonStr(jsonStr, "msisdn");
  if (!parsedMsisdn) {
    return parsedMsisdn.error();
  }
  msisdn = *parsedMsisdn;

  return std::nullopt;
}

std::vector<std::unique_ptr<BaseBinaryInfo>>
SmDeliveryAckRequest::getBinaryValuesInfo() {
  std::vector<std::unique_ptr<BaseBinaryInfo>> valuesInfo{};

  valuesInfo.emplace_back(makeBinaryValue<imsi_t, uint64_t>(
      &mTimsi, Validator::isCorrectImsi, utils::identifierFromStr,
      utils::imsiToStr));

  valuesInfo.emplace_back(
      makeBinaryValue<unsigned int>(&smsId, Validator::isCorrectSmsId));

  valuesInfo.emplace_back(makeBinaryValue<msisdn_t, uint64_t>(
      &msisdn, Validator::isCorrectMsisdn, utils::identifierFromStr,
      [](uint64_t msisdn) { return std::to_string(msisdn); }));

  return valuesInfo;
}

imsi_t SmDeliveryAckRequest::getMTimsi() const { return mTimsi; }

unsigned int SmDeliveryAckRequest::getSmsId() const { return smsId; }

msisdn_t SmDeliveryAckRequest::getMsisdn() const { return msisdn; }
} // namespace common
