#include "rrc_reconfiguration_handover_response.h"

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

std::unique_ptr<BaseJsonInfo>
RrcReconfigurationHandoverResponse::getJsonRootInfo() {
  auto root = makeJsonObject();

  root->addInner("mTimsi",
                 makeJsonValue<imsi_t>(&mTimsi, Validator::isCorrectImsi));

  root->addInner("bsId",
                 makeJsonValue<unsigned int>(&bsId, Validator::isCorrectBsId));

  return root;
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
