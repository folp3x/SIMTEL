#include "sm_transfer_request.h"

#include "common/network/binary_iterator/binary_iterator.h"
#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/network/network.h"

namespace common {
std::string SmTransferRequest::getValuesStr() const {
  std::string showedText = (text.length() > TEXT_CHARS_SHOWED)
                               ? text.substr(0, TEXT_CHARS_SHOWED) + "..."
                               : text;
  return "{mTimsi=" + mTimsi + ", smsId=" + std::to_string(smsId) +
         ", msisdn=" + msisdn + ", text=" + showedText + "}";
}

SmTransferRequest::SmTransferRequest(const imsi_t &mTimsi_, unsigned int smsId_,
                                     const msisdn_t &msisdn_,
                                     const std::string &text_)
    : mTimsi(mTimsi_), smsId(smsId_), msisdn(msisdn_), text(text_) {}

std::string SmTransferRequest::toStr() const {
  std::string showedText = (text.length() > TEXT_CHARS_SHOWED)
                               ? text.substr(0, TEXT_CHARS_SHOWED) + "..."
                               : text;
  return requestTypeToStr(getType()) + getValuesStr();
}

RequestType SmTransferRequest::getType() const {
  return RequestType::SM_Transfer;
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
  auto binMTimsi = BinarySerializer::imsiToBinary(mTimsi);
  if (!binMTimsi) {
    return std::unexpected("IMSI serialize error");
  }
  auto binSmsId = BinarySerializer::toBinary(smsId);
  if (!binSmsId) {
    return std::unexpected("SMS id serialize error");
  }

  auto binMsisdn = BinarySerializer::msisdnToBinary(msisdn);
  if (!binMsisdn) {
    return std::unexpected("MSISDN serialize error");
  }

  auto binText = BinarySerializer::strToBinary(text);

  return mergeBinary(*binMTimsi, *binSmsId, *binMsisdn, binText);
}

std::optional<std::string>
SmTransferRequest::fromBinary(const binary_t &binary) {
  BinaryIterator it{binary};

  auto mTimsiBinary = it.getNext(constants::IMSI_BINARY_BYTES);
  if (!mTimsiBinary) {
    return "Binary too short for m-TIMSI";
  }
  auto parsedMTimsi = BinarySerializer::imsiFromBinary(*mTimsiBinary);
  if (!parsedMTimsi) {
    return "m-TIMSI deserialize error";
  }
  mTimsi = *parsedMTimsi;

  auto smsIdBinary = it.getNext(sizeof(smsId));
  if (!smsIdBinary) {
    return "Binary too short for SMS id";
  }
  auto parsedSmsId = BinarySerializer::fromBinary<unsigned int>(*smsIdBinary);
  if (!parsedSmsId) {
    return "SMS id deserialize error";
  }
  smsId = *parsedSmsId;

  auto msisdnBinary = it.getNext(constants::MSISDN_BINARY_BYTES);
  if (!msisdnBinary) {
    return "Binary too short for MSISDN";
  }
  auto parsedMsisdn = BinarySerializer::msisdnFromBinary(*msisdnBinary);
  if (!parsedMsisdn) {
    return "MSISDN deserialize error";
  }
  msisdn = *parsedMsisdn;

  auto textBinary = it.getRemaining();
  if (!textBinary) {
    return "Binary too short for text";
  }
  text = BinarySerializer::strFromBinary(*textBinary);

  return std::nullopt;
}

imsi_t SmTransferRequest::getMTimsi() const { return mTimsi; }

unsigned int SmTransferRequest::getSmsId() const { return smsId; }

msisdn_t SmTransferRequest::getMsisdn() const { return msisdn; }

std::string SmTransferRequest::getText() const { return text; }
} // namespace common
