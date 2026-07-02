#include "sm_delivery_response.h"

namespace common {
SmDeliveryResponse::SmDeliveryResponse(const imsi_t &mTimsi, unsigned int smsId,
                                       const msisdn_t &msisdn,
                                       const std::string &text)
    : SmTransferRequest(mTimsi, smsId, msisdn, text) {}

RequestType SmDeliveryResponse::getType() const {
  return RequestType::SM_Delivery;
}
} // namespace common
