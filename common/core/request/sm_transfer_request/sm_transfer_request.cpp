#include "sm_transfer_request.h"

#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/str/str.h"
#include "common/validator/validator.h"

namespace common {
std::string SmTransferRequest::getValuesStr() const {
  std::string showedText = (text.length() > TextCharsShowed)
                               ? text.substr(0, TextCharsShowed) + "..."
                               : text;
  return "{mTimsi=" + mTimsi + ", smsId=" + std::to_string(smsId) +
         ", msisdn=" + msisdn + ", text=" + showedText + "}";
}

SmTransferRequest::SmTransferRequest(const imsi_t &mTimsi_, unsigned int smsId_,
                                     const msisdn_t &msisdn_,
                                     const std::string &text_)
    : mTimsi(mTimsi_), smsId(smsId_), msisdn(msisdn_), text(text_) {}

std::string SmTransferRequest::toStr() const {
  return requestTypeToStr(getType()) + getValuesStr();
}

RequestType SmTransferRequest::getType() const {
  return RequestType::SmTransfer;
}

nlohmann::json SmTransferRequest::toJson() const {
  return nlohmann::json{
      {"mTimsi", mTimsi}, {"smsId", smsId}, {"msisdn", msisdn}, {"text", text}};
}

std::optional<std::string>
SmTransferRequest::fromJsonStr(const std::string &jsonStr) {
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

  auto parsedText = JsonDeserializer::strFromJsonStr(jsonStr, "text");
  if (!parsedText) {
    return parsedText.error();
  }
  text = *parsedText;

  return std::nullopt;
}

std::vector<std::unique_ptr<BaseBinaryInfo>>
SmTransferRequest::getBinaryValuesInfo() {
  std::vector<std::unique_ptr<BaseBinaryInfo>> valuesInfo{};

  valuesInfo.emplace_back(makeBinaryValue<imsi_t, uint64_t>(
      &mTimsi, Validator::isCorrectImsi, utils::identifierFromStr,
      utils::imsiToStr));

  valuesInfo.emplace_back(
      makeBinaryValue<unsigned int>(&smsId, Validator::isCorrectSmsId));

  valuesInfo.emplace_back(makeBinaryValue<msisdn_t, uint64_t>(
      &msisdn, Validator::isCorrectMsisdn, utils::identifierFromStr,
      [](uint64_t msisdn) { return std::to_string(msisdn); }));

  valuesInfo.emplace_back(makeBinaryValue(&text));

  return valuesInfo;
}

imsi_t SmTransferRequest::getMTimsi() const { return mTimsi; }

unsigned int SmTransferRequest::getSmsId() const { return smsId; }

msisdn_t SmTransferRequest::getMsisdn() const { return msisdn; }

std::string SmTransferRequest::getText() const { return text; }
} // namespace common
