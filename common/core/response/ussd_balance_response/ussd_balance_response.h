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

  virtual std::unique_ptr<BaseJsonInfo> getJsonRootInfo() override;

  virtual std::vector<std::unique_ptr<BaseBinaryInfo>>
  getBinaryValuesInfo() override;

  double getBalance() const;
};
} // namespace common
