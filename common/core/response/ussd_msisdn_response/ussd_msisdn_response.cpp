#include "ussd_msisdn_response.h"

#include "common/utils/str/str.h"
#include "common/validator/validator.h"

namespace common {
UssdMsisdnResponse::UssdMsisdnResponse(const msisdn_t &msisdn_)
    : msisdn(msisdn_) {}

RequestType UssdMsisdnResponse::getType() const {
  return RequestType::UssdMsisdn;
}

nlohmann::json UssdMsisdnResponse::toJson() const {
  return nlohmann::json{{"msisdn", msisdn}};
}

std::unique_ptr<BaseJsonInfo> UssdMsisdnResponse::getJsonRootInfo() {
  auto root = makeJsonObject();

  root->addInner("msisdn",
                 makeJsonValue<msisdn_t>(&msisdn, Validator::isCorrectMsisdn));

  return root;
}

std::vector<std::unique_ptr<BaseBinaryInfo>>
UssdMsisdnResponse::getBinaryValuesInfo() {
  std::vector<std::unique_ptr<BaseBinaryInfo>> valuesInfo{};

  valuesInfo.emplace_back(makeBinaryValue<msisdn_t, uint64_t>(
      &msisdn, Validator::isCorrectMsisdn, utils::identifierFromStr,
      [](uint64_t msisdn) { return std::to_string(msisdn); }));

  return valuesInfo;
}

msisdn_t UssdMsisdnResponse::getMsisdn() const { return msisdn; }
} // namespace common
