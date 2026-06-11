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

SimtelBaseStation *SimtelUeContext::getBs() const { return bs; }

void SimtelUeContext::setBs(SimtelBaseStation *bs_) { bs = bs_; }

common::Protocol SimtelUeContext::getProtocol() const { return protocol; }

void SimtelUeContext::setProtocol(common::Protocol protocol_) {
  protocol = protocol_;
}

std::optional<std::string> SimtelUeContext::translateToBs() {
  auto binary = sock->receiveMessage();
  if (!binary) {
    return binary.error();
  } else {
    bs->setBuf(*binary);
    return std::nullopt;
  }
}

std::optional<std::string> SimtelUeContext::translateToUe() const {
  auto error = sock->sendMessage(bs->getBuf());
  bs->clearBuf();
  return error;
}
} // namespace server
