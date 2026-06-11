#include "ue_context.h"

#include <stdexcept>

namespace client {
UeContext::UeContext(const common::imsi_t &imsi, const common::imei_t &imei,
                     common::Location<> &location,
                     const common::NetworkAddress &serverAddr_)
    : state(imsi, imei, location), serverAddr(serverAddr_) {}

common::imsi_t UeContext::getImsi() const { return state.imsi; }

common::imei_t UeContext::getImei() const { return state.imei; }

common::imsi_t UeContext::getMTimsi() const { return state.mTimsi; }

void UeContext::setMTimsi(const std::string &mTmsi_) {
  if (state.mTimsi.empty()) {
    state.mTimsi = mTmsi_;
  } else {
    throw std::runtime_error("mTmsi cant be changed");
  }
}

common::Location<> UeContext::getLocation() const { return state.location; }

common::NetworkAddress UeContext::getServerAddr() const { return serverAddr; }

bool UeContext::isInActive() const { return inActive; }

void UeContext::setInActive(bool inActive_) { inActive = inActive_; }

common::Protocol UeContext::getProtocol() const { return state.protocol; }

void UeContext::setProtocol(common::Protocol protocol_) {
  state.protocol = protocol_;
}

UeState UeContext::getState() const { return state; }
} // namespace client
