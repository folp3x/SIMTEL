#pragma once

#include "common/core/request/sm_transfer_request/sm_transfer_request.h"

namespace common {
class SmDeliveryResponse : public SmTransferRequest {
public:
  SmDeliveryResponse() = default;
  SmDeliveryResponse(const imsi_t &mTimsi, unsigned int smsId,
                     const msisdn_t &msisdn, const std::string &text);

  virtual RequestType getType() const override;
};
} // namespace common
