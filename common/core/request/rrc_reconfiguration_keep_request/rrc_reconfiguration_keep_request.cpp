#include "rrc_reconfiguration_keep_request.h"

#include "common/network/binary_serializer/binary_serializer.h"
#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/network/network.h"

namespace common {
RrcReconfigurationKeepRequest::RrcReconfigurationKeepRequest(
    const imei_t &imei_, unsigned int bsId_)
    : imei(imei_), bsId(bsId_) {}

std::string RrcReconfigurationKeepRequest::toStr() const {
  return "{imei=" + imei + ", bsId=" + std::to_string(bsId) + "}";
}

RequestType RrcReconfigurationKeepRequest::getType() const {
  return RequestType::Rrc_Reconfiguration_Keep;
}

nlohmann::json RrcReconfigurationKeepRequest::toJson() const {
  return nlohmann::json{{"imei", imei}, {"bsId", bsId}};
}

std::optional<std::string>
RrcReconfigurationKeepRequest::fromJsonStr(const std::string &jsonStr) {
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
RrcReconfigurationKeepRequest::toBinary() const {
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
RrcReconfigurationKeepRequest::fromBinary(const common::binary_t &binary) {
  auto curByte = binary.begin();

  if (curByte + common::constants::IMEI_BINARY_BYTES > binary.end()) {
    return "Binary too short for IMEI";
  }
  binary_t imeiBinary(curByte, curByte + common::constants::IMEI_BINARY_BYTES);
  auto parsedImei = BinarySerializer::imeiFromBinary(imeiBinary);
  if (!parsedImei) {
    return "IMEI deserialize error";
  }
  imei = *parsedImei;
  curByte += common::constants::IMEI_BINARY_BYTES;

  binary_t bsIdBinary(curByte, binary.end());
  auto parsedBsId = BinarySerializer::fromBinary<unsigned int>(bsIdBinary);
  if (!parsedBsId) {
    return "BS id deserialize error";
  }
  bsId = *parsedBsId;

  return std::nullopt;
}

imei_t RrcReconfigurationKeepRequest::getImei() const { return imei; }

unsigned int RrcReconfigurationKeepRequest::getBsId() const { return bsId; }
} // namespace common
