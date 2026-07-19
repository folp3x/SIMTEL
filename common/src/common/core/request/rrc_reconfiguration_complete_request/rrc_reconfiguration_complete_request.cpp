#include "rrc_reconfiguration_complete_request.h"

#include "common/utils/str/str.h"
#include "common/validator/validator.h"

namespace common {
RrcReconfigurationCompleteRequest::RrcReconfigurationCompleteRequest(
    const imsi_t &mTimsi_)
    : mTimsi(mTimsi_) {}

RequestType RrcReconfigurationCompleteRequest::getType() const {
  return RequestType::RrcReconfigurationComplete;
}

nlohmann::json RrcReconfigurationCompleteRequest::toJson() const {
  return nlohmann::json{{"mTimsi", mTimsi}};
}

std::unique_ptr<BaseJsonInfo>
RrcReconfigurationCompleteRequest::getJsonRootInfo() {
  auto root = makeJsonObject();
  root->addInner("mTimsi",
                 makeJsonValue<imsi_t>(&mTimsi, Validator::isCorrectImsi));

  return root;
}

std::vector<std::unique_ptr<BaseBinaryInfo>>
RrcReconfigurationCompleteRequest::getBinaryValuesInfo() {
  std::vector<std::unique_ptr<BaseBinaryInfo>> valuesInfo{};

  valuesInfo.emplace_back(makeBinaryValue<imsi_t, uint64_t>(
      &mTimsi, Validator::isCorrectImsi, utils::identifierFromStr,
      utils::imsiToStr));

  return valuesInfo;
}

imsi_t RrcReconfigurationCompleteRequest::getMTimsi() const { return mTimsi; }
} // namespace common
