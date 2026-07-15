#include "rrc_reconfiguration_keep_response.h"

#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/str/str.h"
#include "common/validator/validator.h"

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

std::vector<std::unique_ptr<BaseBinaryInfo>>
RrcReconfigurationKeepResponse::getBinaryValuesInfo() {
  std::vector<std::unique_ptr<BaseBinaryInfo>> valuesInfo{};

  valuesInfo.emplace_back(makeBinaryValue<imei_t, uint64_t>(
      &imei, Validator::isCorrectImei, utils::identifierFromStr,
      utils::imeiToStr));

  valuesInfo.emplace_back(
      makeBinaryValue<unsigned int>(&bsId, Validator::isCorrectBsId));

  return valuesInfo;
}

imei_t RrcReconfigurationKeepResponse::getImei() const { return imei; }

unsigned int RrcReconfigurationKeepResponse::getBsId() const { return bsId; }
} // namespace common
