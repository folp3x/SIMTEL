#include "sm_delivery_request.h"

namespace common {
SmDeliveryRequest::SmDeliveryRequest(const imsi_t &mTimsi, unsigned int smsId,
                                     const msisdn_t &msisdn,
                                     const std::string &text)
    : SmTransferRequest(mTimsi, smsId, msisdn, text) {}

std::string SmDeliveryRequest::toStr() const {
  return "SM_Delivery" + getValuesStr();
}

RequestType SmDeliveryRequest::getType() const {
  return RequestType::SM_Delivery;
}
} // namespace common
