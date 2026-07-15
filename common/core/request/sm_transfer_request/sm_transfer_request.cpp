#include "sm_transfer_request.h"

#include "common/network/binary_iterator/binary_iterator.h"
#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/str/str.h"

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

std::expected<binary_t, std::string> SmTransferRequest::toBinary() const {
  binary_t binary;
  BinarySerializer::addToBinary(binary,
                                utils::fromStringSafe<uint64_t>(mTimsi));
  BinarySerializer::addToBinary(binary, smsId);
  BinarySerializer::addToBinary(binary,
                                utils::fromStringSafe<uint64_t>(msisdn));
  BinarySerializer::addToBinary(binary, text);

  return binary;
}

std::optional<std::string>
SmTransferRequest::fromBinary(const binary_t &binary) {
  BinaryIterator it{binary};

  auto mTimsiBinary = it.getNext(constants::ImsiBinaryBytes);
  if (!mTimsiBinary) {
    return "Binary too short for m-TIMSI";
  }
  auto parsedMTimsi = BinarySerializer::fromBinary<uint64_t>(*mTimsiBinary);
  if (!parsedMTimsi) {
    return "m-TIMSI deserialize error";
  }
  mTimsi = utils::imsiToStr(*parsedMTimsi);

  auto smsIdBinary = it.getNext(sizeof(smsId));
  if (!smsIdBinary) {
    return "Binary too short for SMS id";
  }
  auto parsedSmsId = BinarySerializer::fromBinary<unsigned int>(*smsIdBinary);
  if (!parsedSmsId) {
    return "SMS id deserialize error";
  }
  smsId = *parsedSmsId;

  auto msisdnBinary = it.getNext(constants::MsisdnBinaryBytes);
  if (!msisdnBinary) {
    return "Binary too short for MSISDN";
  }
  auto parsedMsisdn = BinarySerializer::fromBinary<uint64_t>(*msisdnBinary);
  if (!parsedMsisdn) {
    return "MSISDN deserialize error";
  }
  msisdn = std::to_string(*parsedMsisdn);

  auto textBinary = it.getRemaining();
  if (!textBinary) {
    return "Binary too short for text";
  }
  auto parsedText = BinarySerializer::fromBinary<std::string>(*textBinary);
  if (!parsedText) {
    return "Text deserialize error";
  }
  text = *parsedText;

  return std::nullopt;
}

imsi_t SmTransferRequest::getMTimsi() const { return mTimsi; }

unsigned int SmTransferRequest::getSmsId() const { return smsId; }

msisdn_t SmTransferRequest::getMsisdn() const { return msisdn; }

std::string SmTransferRequest::getText() const { return text; }
} // namespace common
