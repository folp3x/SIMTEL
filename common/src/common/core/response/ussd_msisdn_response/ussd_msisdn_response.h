#pragma once

#include "common/core/request/request/request.h"

namespace common {
class UssdMsisdnResponse : public Request {
private:
  msisdn_t msisdn = "";

  virtual std::unique_ptr<BaseJsonInfo> getJsonRootInfo() override;

  virtual std::vector<std::unique_ptr<BaseBinaryInfo>>
  getBinaryValuesInfo() override;

public:
  UssdMsisdnResponse() = default;
  explicit UssdMsisdnResponse(const msisdn_t &msisdn_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;

  msisdn_t getMsisdn() const;
};
} // namespace common
