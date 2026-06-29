#include "measurement_report_request.h"

#include "common/network/binary_iterator/binary_iterator.h"
#include "common/network/binary_serializer/binary_serializer.h"
#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/network/network.h"

namespace common {
MeasurementReportRequest::MeasurementReportRequest(const imei_t &imei_,
                                                   const imsi_t &imsi_,
                                                   unsigned int bsId_)
    : imei(imei_), imsi(imsi_), bsId(bsId_) {}

RequestType MeasurementReportRequest::getType() const {
  return RequestType::Measurement_Report;
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
  auto binImei = BinarySerializer::imeiToBinary(imei);
  if (!binImei) {
    return std::unexpected("IMEI serialize error");
  }
  auto binImsi = BinarySerializer::imsiToBinary(imsi);
  if (!binImsi) {
    return std::unexpected("IMSI serialize error");
  }
  auto binBsId = BinarySerializer::toBinary(bsId);
  if (!binBsId) {
    return std::unexpected("BS id serialize error");
  }

  return mergeBinary(*binImei, *binImsi, *binBsId);
}

std::optional<std::string>
MeasurementReportRequest::fromBinary(const binary_t &binary) {
  BinaryIterator it{binary};

  auto imeiBinary = it.getNext(constants::IMEI_BINARY_BYTES);
  if (!imeiBinary) {
    return "Binary too short for IMEI";
  }
  auto parsedImei = BinarySerializer::imeiFromBinary(*imeiBinary);
  if (!parsedImei) {
    return "IMEI deserialize error";
  }
  imei = *parsedImei;

  auto imsiBinary = it.getNext(constants::IMSI_BINARY_BYTES);
  if (!imsiBinary) {
    return "Binary too short for IMSI";
  }
  auto parsedImsi = BinarySerializer::imsiFromBinary(*imsiBinary);
  if (!parsedImsi) {
    return "IMSI deserialize error";
  }
  imsi = *parsedImsi;

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
