#include "ussd_balance_response.h"

#include "common/validator/validator.h"

namespace common {
UssdBalanceResponse::UssdBalanceResponse(double balance_) : balance(balance_) {}

RequestType UssdBalanceResponse::getType() const {
  return RequestType::UssdBalance;
}

nlohmann::json UssdBalanceResponse::toJson() const {
  return nlohmann::json{{"balance", balance}};
}

std::unique_ptr<BaseJsonInfo> UssdBalanceResponse::getJsonRootInfo() {
  auto root = makeJsonObject();
  root->addInner("balance", makeJsonValue(&balance));
  return root;
}

std::vector<std::unique_ptr<BaseBinaryInfo>>
UssdBalanceResponse::getBinaryValuesInfo() {
  std::vector<std::unique_ptr<BaseBinaryInfo>> valuesInfo{};
  valuesInfo.emplace_back(makeBinaryValue(&balance));
  return valuesInfo;
}

double UssdBalanceResponse::getBalance() const { return balance; }
} // namespace common
