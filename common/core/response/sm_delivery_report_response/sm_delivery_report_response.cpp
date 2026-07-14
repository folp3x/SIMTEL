#include "sm_delivery_report_response.h"

#include "common/network/binary_iterator/binary_iterator.h"
#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/str/str.h"

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

std::expected<binary_t, std::string>
SmDeliveryReportResponse::toBinary() const {
  binary_t binary;
  BinarySerializer::addToBinary(binary, fromStringSafe<uint64_t>(mTimsi));
  BinarySerializer::addToBinary(binary, smsId);

  return binary;
}

std::optional<std::string>
SmDeliveryReportResponse::fromBinary(const binary_t &binary) {
  BinaryIterator it{binary};

  auto mTimsiBinary = it.getNext(constants::ImsiBinaryBytes);
  if (!mTimsiBinary) {
    return "Binary too short for m-timsi";
  }
  auto parsedMTimsi = BinarySerializer::fromBinary<uint64_t>(*mTimsiBinary);
  if (!parsedMTimsi) {
    return "m-timsi deserialize error";
  }
  mTimsi = imsiToStr(*parsedMTimsi);

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

imsi_t SmDeliveryReportResponse::getMTimsi() const { return mTimsi; }

unsigned int SmDeliveryReportResponse::getSmsId() const { return smsId; }
} // namespace common
