#pragma once

#include "common/core/request/request/request.h"

namespace common {
class UssdBalanceResponse : public Request {
private:
  double balance = 0;

public:
  UssdBalanceResponse() = default;
  explicit UssdBalanceResponse(double balance_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
  virtual std::optional<std::string> fromJsonStr(const std::string &jsonStr);

  virtual std::expected<binary_t, std::string> toBinary() const;
  virtual std::optional<std::string> fromBinary(const binary_t &binary);

  double getBalance() const;
};
} // namespace common
