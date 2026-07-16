#pragma once

#include "common/core/request/request/request.h"

namespace common {
class SmDeliveryAckRequest : public Request {
private:
  imsi_t mTimsi = "";
  unsigned int smsId = 0;
  msisdn_t msisdn = "";

  virtual std::unique_ptr<BaseJsonInfo> getJsonRootInfo() override;

  virtual std::vector<std::unique_ptr<BaseBinaryInfo>>
  getBinaryValuesInfo() override;

public:
  SmDeliveryAckRequest() = default;
  SmDeliveryAckRequest(const imsi_t &mTimsi_, unsigned int smsId_,
                       const msisdn_t &msisdn_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;

  imsi_t getMTimsi() const;
  unsigned int getSmsId() const;
  msisdn_t getMsisdn() const;
};
} // namespace common
