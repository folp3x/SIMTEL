#include "measurement_control_response.h"

#include "common/network/binary_iterator/binary_iterator.h"
#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/str/str.h"

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

std::expected<binary_t, std::string>
MeasurementControlResponse::toBinary() const {
  binary_t binary;
  BinarySerializer::addToBinary(binary, fromStringSafe<uint64_t>(imei));
  BinarySerializer::addToBinary(binary, signal);
  BinarySerializer::addToBinary(binary, bsId);

  return binary;
}

std::optional<std::string>
MeasurementControlResponse::fromBinary(const binary_t &binary) {
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

  auto signalBinary = it.getNext(sizeof(signal));
  if (!signalBinary) {
    return "Binary too short for signal";
  }
  auto parsedSignal = BinarySerializer::fromBinary<unsigned int>(*signalBinary);
  if (!parsedSignal) {
    return "Signal deserialize error";
  }
  signal = *parsedSignal;

  auto bsIdBinary = it.getNext(sizeof(bsId));
  if (!bsIdBinary) {
    return "Binary too short for bsId";
  }
  auto parsedBsId = BinarySerializer::fromBinary<unsigned int>(*bsIdBinary);
  if (!parsedBsId) {
    return "BS id deserialize error";
  }
  bsId = *parsedBsId;

  return std::nullopt;
}

imei_t MeasurementControlResponse::getImei() const { return imei; }

unsigned int MeasurementControlResponse::getSignal() const { return signal; }

unsigned int MeasurementControlResponse::getBsId() const { return bsId; }
} // namespace common
