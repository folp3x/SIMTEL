#pragma once

#include "common/core/request/sm_transfer_request/sm_transfer_request.h"

namespace common {
class SmDeliveryRequest : public SmTransferRequest {
public:
  SmDeliveryRequest() = default;
  SmDeliveryRequest(const imsi_t &mTimsi, unsigned int smsId,
                    const common::msisdn_t &msisdn, const std::string &text);

  virtual std::string toStr() const override;

  virtual RequestType getType() const override;
};
} // namespace common
