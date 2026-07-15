#include "rrc_reconfiguration_keep_response.h"

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

std::unique_ptr<BaseJsonInfo>
RrcReconfigurationKeepResponse::getJsonRootInfo() {
  auto root = makeJsonObject();

  root->addInner("imei",
                 makeJsonValue<imsi_t>(&imei, Validator::isCorrectImei));

  root->addInner("bsId",
                 makeJsonValue<unsigned int>(&bsId, Validator::isCorrectBsId));

  return root;
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
