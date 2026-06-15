#include "sm_delivery_report_request.h"

#include "common/network/binary_iterator/binary_iterator.h"
#include "common/network/binary_serializer/binary_serializer.h"
#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/network/network.h"

namespace common {
SmDeliveryReportRequest::SmDeliveryReportRequest(const msisdn_t &msisdn_,
                                                 unsigned int smsId_)
    : msisdn(msisdn_), smsId(smsId_) {}

std::string SmDeliveryReportRequest::toStr() const {
  return "SM_Transfer{msisdn=" + msisdn + ", smsId=" + std::to_string(smsId) +
         "}";
}

RequestType SmDeliveryReportRequest::getType() const {
  return RequestType::SM_Delivery_Report;
}

nlohmann::json SmDeliveryReportRequest::toJson() const {
  return nlohmann::json{{"msisdn", msisdn}, {"smsId", smsId}};
}

std::optional<std::string>
SmDeliveryReportRequest::fromJsonStr(const std::string &jsonStr) {
  auto parsedMsisdn = JsonDeserializer::strFromJsonStr(jsonStr, "msisdn");
  if (!parsedMsisdn) {
    return parsedMsisdn.error();
  }
  msisdn = *parsedMsisdn;

  auto parsedSmsId = JsonDeserializer::smsIdFromJsonStr(jsonStr, "smsId");
  if (!parsedSmsId) {
    return parsedSmsId.error();
  }
  smsId = *parsedSmsId;

  return std::nullopt;
}

std::expected<binary_t, std::string> SmDeliveryReportRequest::toBinary() const {
  auto binMsisdn = BinarySerializer::msisdnToBinary(msisdn);
  if (!binMsisdn) {
    return std::unexpected("MSISDN serialize error");
  }

  auto binSmsId = BinarySerializer::toBinary(smsId);
  if (!binSmsId) {
    return std::unexpected("SMS id serialize error");
  }

  return mergeBinary(*binMsisdn, *binSmsId);
}

std::optional<std::string>
SmDeliveryReportRequest::fromBinary(const binary_t &binary) {
  BinaryIterator it{binary};

  auto msisdnBinary = it.getNext(constants::MSISDN_BINARY_BYTES);
  if (!msisdnBinary) {
    return "Binary too short for MSISDN";
  }
  auto parsedMsisdn = BinarySerializer::msisdnFromBinary(*msisdnBinary);
  if (!parsedMsisdn) {
    return "MSISDN deserialize error";
  }
  msisdn = *parsedMsisdn;

  auto smsIdBinary = it.getNext(sizeof(smsId));
  if (!smsIdBinary) {
    return "Binary too short for SMS id";
  }
  auto parsedSmsId = BinarySerializer::fromBinary<unsigned int>(*smsIdBinary);
  if (!parsedSmsId) {
    return "SMS id deserialize error";
  }
  smsId = *parsedSmsId;

  return std::nullopt;
}

msisdn_t SmDeliveryReportRequest::getMsisdn() const { return msisdn; }

unsigned int SmDeliveryReportRequest::getSmsId() const { return smsId; }
} // namespace common
