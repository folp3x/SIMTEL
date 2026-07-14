#include "measurement_report_request.h"

#include "common/network/binary_iterator/binary_iterator.h"
#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/str/str.h"

namespace common {
MeasurementReportRequest::MeasurementReportRequest(const imei_t &imei_,
                                                   const imsi_t &imsi_,
                                                   unsigned int bsId_)
    : imei(imei_), imsi(imsi_), bsId(bsId_) {}

RequestType MeasurementReportRequest::getType() const {
  return RequestType::MeasurementReport;
}

nlohmann::json MeasurementReportRequest::toJson() const {
  return nlohmann::json{{"imei", imei}, {"imsi", imsi}, {"bsId", bsId}};
}

std::optional<std::string>
MeasurementReportRequest::fromJsonStr(const std::string &jsonStr) {
  auto parsedImei = JsonDeserializer::imeiFromJsonStr(jsonStr, "imei");
  if (!parsedImei) {
    return parsedImei.error();
  }
  imei = *parsedImei;

  auto parsedImsi = JsonDeserializer::imsiFromJsonStr(jsonStr, "imsi");
  if (!parsedImsi) {
    return parsedImsi.error();
  }
  imsi = *parsedImsi;

  auto parsedBsId = JsonDeserializer::bsIdFromJsonStr(jsonStr, "bsId");
  if (!parsedBsId) {
    return parsedBsId.error();
  }
  bsId = *parsedBsId;

  return std::nullopt;
}

std::expected<binary_t, std::string>
MeasurementReportRequest::toBinary() const {
  binary_t binary;
  BinarySerializer::addToBinary(binary, fromStringSafe<uint64_t>(imei));
  BinarySerializer::addToBinary(binary, fromStringSafe<uint64_t>(imsi));
  BinarySerializer::addToBinary(binary, bsId);

  return binary;
}

std::optional<std::string>
MeasurementReportRequest::fromBinary(const binary_t &binary) {
  BinaryIterator it{binary};

  auto imeiBinary = it.getNext(constants::ImeiBinaryBytes);
  if (!imeiBinary) {
    return "Binary too short for IMEI";
  }
  auto parsedImei = BinarySerializer::fromBinary<uint64_t>(*imeiBinary);
  if (!parsedImei) {
    return "IMEI deserialize error";
  }
  imei = imeiToStr(*parsedImei);

  auto imsiBinary = it.getNext(constants::ImsiBinaryBytes);
  if (!imsiBinary) {
    return "Binary too short for IMSI";
  }
  auto parsedImsi = BinarySerializer::fromBinary<uint64_t>(*imsiBinary);
  if (!parsedImsi) {
    return "IMSI deserialize error";
  }
  imsi = imsiToStr(*parsedImsi);

  auto bsIdBinary = it.getNext(sizeof(bsId));
  if (!bsIdBinary) {
    return "Binary too short for BS id";
  }
  auto parsedBsId = BinarySerializer::fromBinary<unsigned int>(*bsIdBinary);
  if (!parsedBsId) {
    return "BS id deserialize error";
  }
  bsId = *parsedBsId;

  return std::nullopt;
}

imei_t MeasurementReportRequest::getImei() const { return imei; }

imei_t MeasurementReportRequest::getImsi() const { return imsi; }

unsigned int MeasurementReportRequest::getBsId() const { return bsId; }
} // namespace common
