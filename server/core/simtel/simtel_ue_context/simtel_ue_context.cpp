#include "simtel_ue_context.h"

#include "common/core/request/request_serializer/request_serializer.h"
#include "common/network/binary_serializer/binary_serializer.h"
#include "common/utils/network/network.h"
#include "server/core/simtel/simtel_base_station/simtel_base_station.h"

namespace server {
SimtelUeContext::SimtelUeContext(std::unique_ptr<Socket> sock_)
    : sock(std::move(sock_)) {}

common::imsi_t SimtelUeContext::getImsi() const { return imsi; }

void SimtelUeContext::setImsi(const common::imsi_t &imsi_) { imsi = imsi_; }

void SimtelUeContext::setBs(SimtelBaseStation *bs_) { bs = bs_; }

void SimtelUeContext::receiveLocationUpdate() {
  auto receiveResult = sock->receiveMessage();
  if (receiveResult) {
    resendToBs(*receiveResult);
  }
}

void SimtelUeContext::resendToUe(const common::binary_t &binary) const {
  sock->sendMessage(binary);
}

void SimtelUeContext::resendToBs(const common::binary_t &binary) const {
  bs->setBuf(binary);
}
} // namespace server
