#include "simtel_ue_context.h"

#include "common/core/request/request_serializer/request_serializer.h"
#include "common/network/binary_serializer/binary_serializer.h"
#include "common/utils/network/network.h"
#include "server/core/simtel/simtel_base_station/simtel_base_station.h"

namespace server {
SimtelUeContext::SimtelUeContext(std::unique_ptr<Socket> sock_)
    : sock(std::move(sock_)) {}

SimtelBaseStation *SimtelUeContext::getBs() const { return bs; }

void SimtelUeContext::setBs(SimtelBaseStation *bs_) { bs = bs_; }

common::Protocol SimtelUeContext::getProtocol() const { return protocol; }

void SimtelUeContext::setProtocol(common::Protocol protocol_) {
  protocol = protocol_;
}

common::binary_t SimtelUeContext::takeBuf() {
  auto copy = buf;
  buf.clear();
  return copy;
}

void SimtelUeContext::setBuf(const common::binary_t &buf_) { buf = buf_; }

std::optional<std::string> SimtelUeContext::receiveData() {
  auto binary = sock->receiveMessage();
  if (!binary) {
    return binary.error();
  } else {
    buf = std::move(*binary);
    return std::nullopt;
  }
}

std::optional<std::string> SimtelUeContext::sendBufToUe() {
  auto error = sock->sendMessage(buf);
  buf.clear();
  return error;
}

std::string SimtelUeContext::getAddrStr() const { return sock->getAddrStr(); }
} // namespace server
