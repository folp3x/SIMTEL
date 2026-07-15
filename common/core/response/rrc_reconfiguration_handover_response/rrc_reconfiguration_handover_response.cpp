#include "rrc_reconfiguration_handover_response.h"

#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/str/str.h"
#include "common/validator/validator.h"

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

std::vector<std::unique_ptr<BaseBinaryInfo>>
RrcReconfigurationHandoverResponse::getBinaryValuesInfo() {
  std::vector<std::unique_ptr<BaseBinaryInfo>> valuesInfo{};

  valuesInfo.emplace_back(makeBinaryValue<imsi_t, uint64_t>(
      &mTimsi, Validator::isCorrectImsi, utils::identifierFromStr,
      utils::imeiToStr));

  valuesInfo.emplace_back(
      makeBinaryValue<unsigned int>(&bsId, Validator::isCorrectBsId));

  return valuesInfo;
}

imsi_t RrcReconfigurationHandoverResponse::getMTimsi() const { return mTimsi; }

unsigned int RrcReconfigurationHandoverResponse::getBsId() const {
  return bsId;
}
} // namespace common
