#include "ussd_balance_response.h"

#include "common/network/binary_iterator/binary_iterator.h"
#include "common/network/json_deserializer/json_deserializer.h"

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

std::expected<binary_t, std::string> UssdBalanceResponse::toBinary() const {
  binary_t binary;
  BinarySerializer::addToBinary(binary, balance);

  return binary;
}

std::optional<std::string>
UssdBalanceResponse::fromBinary(const binary_t &binary) {
  BinaryIterator it{binary};

  auto balanceBinary = it.getNext(sizeof(balance));
  if (!balanceBinary) {
    return "Binary too short for balance";
  }
  auto parsedBalance = BinarySerializer::fromBinary<double>(*balanceBinary);
  if (!parsedBalance) {
    return "Balance deserialize error";
  }
  balance = *parsedBalance;

  return std::nullopt;
}

double UssdBalanceResponse::getBalance() const { return balance; }
} // namespace common
