#include "ue_context.h"

namespace client {
UeContext::UeContext(common::Location<> &location_, const common::imsi_t &imsi_,
                     const common::imei_t &imei_,
                     const common::NetworkAddress &serverAddr_)
    : location(location_), imsi(imsi_), imei(imei_), serverAddr(serverAddr_) {}

common::imsi_t UeContext::getImsi() const { return imsi; }

common::imei_t UeContext::getImei() const { return imei; }

common::imsi_t UeContext::getMtimsi() const { return mtimsi; }

void UeContext::setMtmsi(const std::string &mtmsi_) {
  if (!mtimsi.empty()) {
    mtimsi = mtmsi_;
  }
}

common::Location<> UeContext::getLocation() const { return location; }

common::NetworkAddress UeContext::getServerAddr() const { return serverAddr; }

bool UeContext::isInActive() const { return inActive; }

void UeContext::setInActive(bool inActive_) { inActive = inActive_; }

common::Protocol UeContext::getProtocol() const { return protocol; }

void UeContext::setProtocol(common::Protocol protocol_) {
  protocol = protocol_;
}
} // namespace client
