#pragma once

#include "common/core/request/sm_delivery_report_request/sm_delivery_report_request.h"

namespace common {
class SmDeliveryErrorRequest : public SmDeliveryReportRequest {
public:
  SmDeliveryErrorRequest() = default;
  SmDeliveryErrorRequest(const common::imsi_t &mTimsi, unsigned int smsId);

  virtual RequestType getType() const override;
};
} // namespace common
