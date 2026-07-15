#include "ussd_code_request.h"

#include "common/utils/num/num.h"
#include "common/utils/str/str.h"

#include "common/validator/validator.h"

namespace common {
UssdCodeRequest::UssdCodeRequest(const imsi_t &mTimsi_, uint8_t code_)
    : mTimsi(mTimsi_), code(code_) {}

RequestType UssdCodeRequest::getType() const { return RequestType::UssdCode; }

nlohmann::json UssdCodeRequest::toJson() const {
  return nlohmann::json{{"mTimsi", mTimsi}, {"code", code}};
}

std::unique_ptr<BaseJsonInfo> UssdCodeRequest::getJsonRootInfo() {
  auto root = makeJsonObject();

  root->addInner("mTimsi",
                 makeJsonValue<imsi_t>(&mTimsi, Validator::isCorrectImsi));

  root->addInner("code", makeJsonValue(&code));

  return root;
}

std::vector<std::unique_ptr<BaseBinaryInfo>>
UssdCodeRequest::getBinaryValuesInfo() {
  std::vector<std::unique_ptr<BaseBinaryInfo>> valuesInfo{};

  valuesInfo.emplace_back(makeBinaryValue<imsi_t, uint64_t>(
      &mTimsi, Validator::isCorrectImsi, utils::identifierFromStr,
      utils::imsiToStr));

  valuesInfo.emplace_back(makeBinaryValue<uint8_t>(&code));

  return valuesInfo;
}

imsi_t UssdCodeRequest::getMTimsi() const { return mTimsi; }

uint8_t UssdCodeRequest::getCode() const { return code; }
} // namespace common
