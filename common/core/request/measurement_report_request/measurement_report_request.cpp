#include "measurement_report_request.h"

#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/str/str.h"
#include "common/validator/validator.h"

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

std::vector<std::unique_ptr<BaseBinaryInfo>>
MeasurementReportRequest::getBinaryValuesInfo() {
  std::vector<std::unique_ptr<BaseBinaryInfo>> valuesInfo{};

  valuesInfo.emplace_back(makeBinaryValue<imei_t, uint64_t>(
      &imei, Validator::isCorrectImei, utils::identifierFromStr,
      utils::imeiToStr));

  valuesInfo.emplace_back(makeBinaryValue<imsi_t, uint64_t>(
      &imsi, Validator::isCorrectImsi, utils::identifierFromStr,
      utils::imsiToStr));

  valuesInfo.emplace_back(
      makeBinaryValue<unsigned int>(&bsId, Validator::isCorrectBsId));

  return valuesInfo;
}

imei_t MeasurementReportRequest::getImei() const { return imei; }

imei_t MeasurementReportRequest::getImsi() const { return imsi; }

unsigned int MeasurementReportRequest::getBsId() const { return bsId; }
} // namespace common
