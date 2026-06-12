#include "simtel_ue_context.h"

#include "common/core/request/request_serializer/request_serializer.h"
#include "common/network/binary_serializer/binary_serializer.h"
#include "common/utils/network/network.h"
#include "server/core/simtel/simtel_base_station/simtel_base_station.h"

namespace server {
SimtelUeContext::SimtelUeContext(std::unique_ptr<Socket> sock_)
    : sock(std::move(sock_)) {}

common::imsi_t SimtelUeContext::getMTimsi() const { return mTimsi; }

bool SimtelUeContext::setMTimsi(const common::imsi_t &mTimsi_) {
  if (!mTimsiSet) {
    mTimsi = mTimsi_;
    mTimsiSet = true;
    return true;
  }
  return false;
}

SimtelBaseStation *SimtelUeContext::getBs() const { return bs; }

void SimtelUeContext::setBs(SimtelBaseStation *bs_) { bs = bs_; }

common::Protocol SimtelUeContext::getProtocol() const { return protocol; }

void SimtelUeContext::setProtocol(common::Protocol protocol_) {
  protocol = protocol_;
}

common::binary_t SimtelUeContext::takeBuf() {
  std::unique_lock lock(bufMtx);
  auto copy = buf;
  buf.clear();
  bufCv.notify_one();
  return copy;
}

void SimtelUeContext::setBuf(const common::binary_t &buf_) {
  std::unique_lock lock(bufMtx);
  bufCv.wait(lock, [this] { return buf.empty(); });
  buf = buf_;
  MessageHolder::instance().addMsg(toStr() + " buf set (" +
                                   std::to_string(buf.size()) + " bytes)");
}

std::optional<common::NetworkError> SimtelUeContext::receiveData() {
  auto binary = sock->receiveMessage();
  if (!binary) {
    return binary.error();
  } else {
    buf = std::move(*binary);
    return std::nullopt;
  }
}

std::optional<common::NetworkError> SimtelUeContext::sendBufToUe() {
  auto error = sock->sendMessage(buf);
  buf.clear();
  return error;
}

std::string SimtelUeContext::toStr() const {
  return "UE_" + sock->getAddrStr();
}
} // namespace server
