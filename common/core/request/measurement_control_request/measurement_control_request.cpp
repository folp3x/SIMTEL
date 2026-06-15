#include "measurement_control_request.h"

#include "common/network/binary_iterator/binary_iterator.h"
#include "common/network/binary_serializer/binary_serializer.h"
#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/network/network.h"

namespace common {
MeasurementControlRequest::MeasurementControlRequest(const imei_t &imei_,
                                                     unsigned int signal_,
                                                     unsigned int bsId_)
    : imei(imei_), signal(signal_), bsId(bsId_) {}

std::string MeasurementControlRequest::toStr() const {
  return "Measurement_Control{imei=" + imei +
         ", signal=" + std::to_string(signal) +
         ", bsId=" + std::to_string(bsId) + "}";
}

RequestType MeasurementControlRequest::getType() const {
  return RequestType::Measurement_Control;
}

nlohmann::json MeasurementControlRequest::toJson() const {
  return nlohmann::json{{"imei", imei}, {"signal", signal}, {"bsId", bsId}};
}

std::optional<std::string>
MeasurementControlRequest::fromJsonStr(const std::string &jsonStr) {
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
MeasurementControlRequest::toBinary() const {
  auto binImei = BinarySerializer::imeiToBinary(imei);
  if (!binImei) {
    return std::unexpected("IMEI serialize error");
  }
  auto binSignal = BinarySerializer::toBinary(signal);
  if (!binSignal) {
    return std::unexpected("Signal serialize error");
  }
  auto binBsId = BinarySerializer::toBinary(bsId);
  if (!binBsId) {
    return std::unexpected("BS id serialize error");
  }

  return mergeBinary(*binImei, *binSignal, *binBsId);
}

std::optional<std::string>
MeasurementControlRequest::fromBinary(const binary_t &binary) {
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

imei_t MeasurementControlRequest::getImei() const { return imei; }

unsigned int MeasurementControlRequest::getSignal() const { return signal; }

unsigned int MeasurementControlRequest::getBsId() const { return bsId; }
} // namespace common
