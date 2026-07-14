#include "rrc_reconfiguration_keep_response.h"

#include "common/network/binary_iterator/binary_iterator.h"
#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/str/str.h"

namespace common {
RrcReconfigurationKeepResponse::RrcReconfigurationKeepResponse(
    const imei_t &imei_, unsigned int bsId_)
    : imei(imei_), bsId(bsId_) {}

RequestType RrcReconfigurationKeepResponse::getType() const {
  return RequestType::RrcReconfigurationKeep;
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
  binary_t binary;
  BinarySerializer::addToBinary(binary, fromStringSafe<uint64_t>(imei));
  BinarySerializer::addToBinary(binary, bsId);

  return binary;
}

std::optional<std::string>
RrcReconfigurationKeepResponse::fromBinary(const binary_t &binary) {
  BinaryIterator it{binary};

  auto imeiBinary = it.getNext(common::constants::ImeiBinaryBytes);
  if (!imeiBinary) {
    return "Binary too short for IMEI";
  }
  auto parsedImei = BinarySerializer::fromBinary<uint64_t>(*imeiBinary);
  if (!parsedImei) {
    return "IMEI deserialize error";
  }
  imei = imeiToStr(*parsedImei);

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
