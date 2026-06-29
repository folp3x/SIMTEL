#include "sm_delivery_report_request.h"

#include "common/network/binary_iterator/binary_iterator.h"
#include "common/network/binary_serializer/binary_serializer.h"
#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/network/network.h"

namespace common {
SmDeliveryReportRequest::SmDeliveryReportRequest(const imsi_t &mTimsi_,
                                                 unsigned int smsId_)
    : mTimsi(mTimsi_), smsId(smsId_) {}

RequestType SmDeliveryReportRequest::getType() const {
  return RequestType::SM_Delivery_Report;
}

nlohmann::json SmDeliveryReportRequest::toJson() const {
  return nlohmann::json{{"mTimsi", mTimsi}, {"smsId", smsId}};
}

std::optional<std::string>
SmDeliveryReportRequest::fromJsonStr(const std::string &jsonStr) {
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

std::expected<binary_t, std::string> SmDeliveryReportRequest::toBinary() const {
  auto binMTimsi = BinarySerializer::imsiToBinary(mTimsi);
  if (!binMTimsi) {
    return std::unexpected("m-imsi serialize error");
  }

  auto binSmsId = BinarySerializer::toBinary(smsId);
  if (!binSmsId) {
    return std::unexpected("SMS id serialize error");
  }

  return mergeBinary(*binMTimsi, *binSmsId);
}

std::optional<std::string>
SmDeliveryReportRequest::fromBinary(const binary_t &binary) {
  BinaryIterator it{binary};

  auto mTimsiBinary = it.getNext(constants::IMSI_BINARY_BYTES);
  if (!mTimsiBinary) {
    return "Binary too short for m-timsi";
  }
  auto parsedMTimsi = BinarySerializer::imsiFromBinary(*mTimsiBinary);
  if (!parsedMTimsi) {
    return "m-timsi deserialize error";
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

  return std::nullopt;
}

imsi_t SmDeliveryReportRequest::getMTimsi() const { return mTimsi; }

unsigned int SmDeliveryReportRequest::getSmsId() const { return smsId; }
} // namespace common
