#include "sm_delivery_error_request.h"

namespace common {
SmDeliveryErrorRequest::SmDeliveryErrorRequest(const common::imsi_t &mTimsi,
                                               unsigned int smsId)
    : SmDeliveryReportRequest(mTimsi, smsId) {}

RequestType SmDeliveryErrorRequest::getType() const {
  return RequestType::SM_Delivery_Error;
}
} // namespace common
