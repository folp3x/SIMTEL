#include "rrc_reconfiguration_handover_response.h"

#include "common/network/binary_iterator/binary_iterator.h"
#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/str/str.h"

namespace common {
RrcReconfigurationHandoverResponse::RrcReconfigurationHandoverResponse(
    const imsi_t &mTimsi_, unsigned int bsId_)
    : mTimsi(mTimsi_), bsId(bsId_) {}

RequestType RrcReconfigurationHandoverResponse::getType() const {
  return RequestType::RrcReconfigurationHandover;
}

nlohmann::json RrcReconfigurationHandoverResponse::toJson() const {
  return nlohmann::json{{"mTimsi", mTimsi}, {"bsId", bsId}};
}

std::optional<std::string>
RrcReconfigurationHandoverResponse::fromJsonStr(const std::string &jsonStr) {
  auto parsedMTimsi = JsonDeserializer::imsiFromJsonStr(jsonStr, "mTimsi");
  if (!parsedMTimsi) {
    return parsedMTimsi.error();
  }
  mTimsi = *parsedMTimsi;

  auto parsedBsId = JsonDeserializer::bsIdFromJsonStr(jsonStr, "bsId");
  if (!parsedBsId) {
    return parsedBsId.error();
  }
  bsId = *parsedBsId;

  return std::nullopt;
}

std::expected<binary_t, std::string>
RrcReconfigurationHandoverResponse::toBinary() const {
  binary_t binary;
  BinarySerializer::addToBinary(binary,
                                utils::fromStringSafe<uint64_t>(mTimsi));
  BinarySerializer::addToBinary(binary, bsId);

  return binary;
}

std::optional<std::string>
RrcReconfigurationHandoverResponse::fromBinary(const binary_t &binary) {
  BinaryIterator it{binary};

  auto mTimsiBinary = it.getNext(constants::ImsiBinaryBytes);
  if (!mTimsiBinary) {
    return "Binary too short for m-timsi";
  }
  auto parsedMTimsi = BinarySerializer::fromBinary<uint64_t>(*mTimsiBinary);
  if (!parsedMTimsi) {
    return "m-TIMSI deserialize error";
  }
  mTimsi = utils::imsiToStr(*parsedMTimsi);

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

imsi_t RrcReconfigurationHandoverResponse::getMTimsi() const { return mTimsi; }

unsigned int RrcReconfigurationHandoverResponse::getBsId() const {
  return bsId;
}
} // namespace common
