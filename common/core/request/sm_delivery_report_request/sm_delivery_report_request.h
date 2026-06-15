#pragma once

#include "common/core/request/request/request.h"

namespace common {
class SmDeliveryReportRequest : public Request {
private:
  msisdn_t msisdn = "";
  unsigned int smsId = 0;

public:
  SmDeliveryReportRequest() = default;
  SmDeliveryReportRequest(const common::msisdn_t &msisdn_, unsigned int smsId_);

  virtual std::string toStr() const override;

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
  virtual std::optional<std::string> fromJsonStr(const std::string &jsonStr);

  virtual std::expected<binary_t, std::string> toBinary() const;
  virtual std::optional<std::string> fromBinary(const binary_t &binary);

  msisdn_t getMsisdn() const;
  unsigned int getSmsId() const;
};
} // namespace common
