#include "sm_delivery_ack_request.h"

#include "common/network/binary_iterator/binary_iterator.h"
#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/str/str.h"

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

std::expected<binary_t, std::string> SmDeliveryAckRequest::toBinary() const {
  binary_t binary;
  BinarySerializer::addToBinary(binary,
                                utils::fromStringSafe<uint64_t>(mTimsi));
  BinarySerializer::addToBinary(binary, smsId);
  BinarySerializer::addToBinary(binary,
                                utils::fromStringSafe<uint64_t>(msisdn));

  return binary;
}

std::optional<std::string>
SmDeliveryAckRequest::fromBinary(const binary_t &binary) {
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

  return std::nullopt;
}

imsi_t SmDeliveryAckRequest::getMTimsi() const { return mTimsi; }

unsigned int SmDeliveryAckRequest::getSmsId() const { return smsId; }

msisdn_t SmDeliveryAckRequest::getMsisdn() const { return msisdn; }
} // namespace common
