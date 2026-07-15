#include "measurement_control_response.h"

#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/str/str.h"
#include "common/validator/validator.h"

namespace common {
MeasurementControlResponse::MeasurementControlResponse(const imei_t &imei_,
                                                       unsigned int signal_,
                                                       unsigned int bsId_)
    : imei(imei_), signal(signal_), bsId(bsId_) {}

RequestType MeasurementControlResponse::getType() const {
  return RequestType::MeasurementControl;
}

nlohmann::json MeasurementControlResponse::toJson() const {
  return nlohmann::json{{"imei", imei}, {"signal", signal}, {"bsId", bsId}};
}

std::optional<std::string>
MeasurementControlResponse::fromJsonStr(const std::string &jsonStr) {
  auto parsedImei = JsonDeserializer::imeiFromJsonStr(jsonStr, "imei");
  if (!parsedImei) {
    return parsedImei.error();
  }
  imei = *parsedImei;

  auto parsedSignal = JsonDeserializer::signalFromJsonStr(jsonStr, "signal");
  if (!parsedSignal) {
    return parsedSignal.error();
  }
  signal = *parsedSignal;

  auto parsedBsId = JsonDeserializer::bsIdFromJsonStr(jsonStr, "bsId");
  if (!parsedBsId) {
    return parsedBsId.error();
  }
  bsId = *parsedBsId;

  return std::nullopt;
}

std::vector<std::unique_ptr<BaseBinaryInfo>>
MeasurementControlResponse::getBinaryValuesInfo() {
  std::vector<std::unique_ptr<BaseBinaryInfo>> valuesInfo{};

  valuesInfo.emplace_back(makeBinaryValue<imei_t, uint64_t>(
      &imei, Validator::isCorrectImei, utils::identifierFromStr,
      utils::imeiToStr));

  valuesInfo.emplace_back(
      makeBinaryValue<unsigned int>(&signal, Validator::isCorrectSignal));

  valuesInfo.emplace_back(
      makeBinaryValue<unsigned int>(&bsId, Validator::isCorrectBsId));

  return valuesInfo;
}

imei_t MeasurementControlResponse::getImei() const { return imei; }

unsigned int MeasurementControlResponse::getSignal() const { return signal; }

unsigned int MeasurementControlResponse::getBsId() const { return bsId; }
} // namespace common
