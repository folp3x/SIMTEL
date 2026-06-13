#include "rrc_reconfiguration_handover_request.h"

#include "common/network/binary_serializer/binary_serializer.h"
#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/network/network.h"

namespace common {
RrcReconfigurationHandoverRequest::RrcReconfigurationHandoverRequest(
    const imsi_t &mTimsi_, unsigned int bsId_)
    : mTimsi(mTimsi_), bsId(bsId_) {}

std::string RrcReconfigurationHandoverRequest::toStr() const {
  return "Rrc_Reconfiguration_Handover{mTimsi=" + mTimsi +
         ", bsId=" + std::to_string(bsId) + "}";
}

RequestType RrcReconfigurationHandoverRequest::getType() const {
  return RequestType::Rrc_Reconfiguration_Handover;
}

nlohmann::json RrcReconfigurationHandoverRequest::toJson() const {
  return nlohmann::json{{"mTimsi", mTimsi}, {"bsId", bsId}};
}

std::optional<std::string>
RrcReconfigurationHandoverRequest::fromJsonStr(const std::string &jsonStr) {
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
RrcReconfigurationHandoverRequest::toBinary() const {
  auto binMTimsi = BinarySerializer::imsiToBinary(mTimsi);
  if (!binMTimsi) {
    return std::unexpected("IMSI serialize error");
  }
  auto binBsId = BinarySerializer::toBinary(bsId);
  if (!binBsId) {
    return std::unexpected("BS id serialize error");
  }

  return mergeBinary(*binMTimsi, *binBsId);
}

std::optional<std::string>
RrcReconfigurationHandoverRequest::fromBinary(const common::binary_t &binary) {
  auto curByte = binary.begin();

  if (curByte + common::constants::IMSI_BINARY_BYTES > binary.end()) {
    return "Binary too short for m-TIMSI";
  }
  binary_t mTimsiBinary(curByte,
                        curByte + common::constants::IMSI_BINARY_BYTES);
  auto parsedMTimsi = BinarySerializer::imsiFromBinary(mTimsiBinary);
  if (!parsedMTimsi) {
    return "m-TIMSI deserialize error";
  }
  mTimsi = *parsedMTimsi;
  curByte += common::constants::IMSI_BINARY_BYTES;

  binary_t bsIdBinary(curByte, binary.end());
  auto parsedBsId = BinarySerializer::fromBinary<unsigned int>(bsIdBinary);
  if (!parsedBsId) {
    return "BS id deserialize error";
  }
  bsId = *parsedBsId;

  return std::nullopt;
}
} // namespace common
