#include "ussd_balance_response.h"

#include "common/network/json_deserializer/json_deserializer.h"
#include "common/validator/validator.h"

namespace common {
UssdBalanceResponse::UssdBalanceResponse(double balance_) : balance(balance_) {}

RequestType UssdBalanceResponse::getType() const {
  return RequestType::UssdBalance;
}

nlohmann::json UssdBalanceResponse::toJson() const {
  return nlohmann::json{{"balance", balance}};
}

std::optional<std::string>
UssdBalanceResponse::fromJsonStr(const std::string &jsonStr) {
  auto parsedBalance = JsonDeserializer::balanceFromJsonStr(jsonStr, "balance");
  if (!parsedBalance) {
    return parsedBalance.error();
  }
  balance = *parsedBalance;

  return std::nullopt;
}

std::vector<std::unique_ptr<BaseBinaryInfo>>
UssdBalanceResponse::getBinaryValuesInfo() {
  std::vector<std::unique_ptr<BaseBinaryInfo>> valuesInfo{};
  valuesInfo.emplace_back(makeBinaryValue(&balance));
  return valuesInfo;
}

double UssdBalanceResponse::getBalance() const { return balance; }
} // namespace common
