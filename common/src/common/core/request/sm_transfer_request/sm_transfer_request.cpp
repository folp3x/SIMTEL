#include "sm_transfer_request.h"

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

std::unique_ptr<BaseJsonInfo> SmTransferRequest::getJsonRootInfo() {
  auto root = makeJsonObject();

  root->addInner("mTimsi",
                 makeJsonValue<imsi_t>(&mTimsi, Validator::isCorrectImsi));

  root->addInner(
      "smsId", makeJsonValue<unsigned int>(&smsId, Validator::isCorrectSmsId));

  root->addInner("msisdn",
                 makeJsonValue<msisdn_t>(&msisdn, Validator::isCorrectMsisdn));

  root->addInner("text", makeJsonValue(&text));

  return root;
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
