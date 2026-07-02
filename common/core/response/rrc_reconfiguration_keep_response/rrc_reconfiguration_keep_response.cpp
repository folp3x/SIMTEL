#include "rrc_reconfiguration_keep_response.h"

#include "common/network/binary_iterator/binary_iterator.h"
#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/network/network.h"

namespace common {
RrcReconfigurationKeepResponse::RrcReconfigurationKeepResponse(
    const imei_t &imei_, unsigned int bsId_)
    : imei(imei_), bsId(bsId_) {}

RequestType RrcReconfigurationKeepResponse::getType() const {
  return RequestType::Rrc_Reconfiguration_Keep;
}

nlohmann::json RrcReconfigurationKeepResponse::toJson() const {
  return nlohmann::json{{"imei", imei}, {"bsId", bsId}};
}

std::optional<std::string>
RrcReconfigurationKeepResponse::fromJsonStr(const std::string &jsonStr) {
  auto parsedImei = JsonDeserializer::imeiFromJsonStr(jsonStr, "imei");
  if (!parsedImei) {
    return parsedImei.error();
  }
  imei = *parsedImei;

  auto parsedBsId = JsonDeserializer::bsIdFromJsonStr(jsonStr, "bsId");
  if (!parsedBsId) {
    return parsedBsId.error();
  }
  bsId = *parsedBsId;

  return std::nullopt;
}

std::expected<binary_t, std::string>
RrcReconfigurationKeepResponse::toBinary() const {
  auto binImei = BinarySerializer::imeiToBinary(imei);
  if (!binImei) {
    return std::unexpected("IMEI serialize error");
  }
  auto binBsId = BinarySerializer::toBinary(bsId);
  if (!binBsId) {
    return std::unexpected("BS id serialize error");
  }

  return mergeBinary(*binImei, *binBsId);
}

std::optional<std::string>
RrcReconfigurationKeepResponse::fromBinary(const binary_t &binary) {
  BinaryIterator it{binary};

  auto imeiBinary = it.getNext(common::constants::IMEI_BINARY_BYTES);
  if (!imeiBinary) {
    return "Binary too short for IMEI";
  }
  auto parsedImei = BinarySerializer::imeiFromBinary(*imeiBinary);
  if (!parsedImei) {
    return "IMEI deserialize error";
  }
  imei = *parsedImei;

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

imei_t RrcReconfigurationKeepResponse::getImei() const { return imei; }

unsigned int RrcReconfigurationKeepResponse::getBsId() const { return bsId; }
} // namespace common
