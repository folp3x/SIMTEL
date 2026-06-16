#include "simtel_ue_context.h"

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
  bufEmptyCv.notify_one();

  return copy;
}

void SimtelUeContext::setBuf(const common::binary_t &buf_) {
  std::unique_lock lock(bufMtx);
  bufEmptyCv.wait(lock, [this] { return buf.empty(); });
  buf = buf_;
  MessageHolder::instance().addMsg(toStr() + " buf set (" +
                                   std::to_string(buf.size()) + " bytes)");
}

common::binary_t SimtelUeContext::copyBuf() const {
  std::lock_guard lock(bufMtx);
  MessageHolder::instance().addMsg(toStr() + " buf copied (" +
                                   std::to_string(buf.size()) + " bytes)");
  return buf;
}

void SimtelUeContext::clearBuf() {
  std::lock_guard lock(bufMtx);
  MessageHolder::instance().addMsg(toStr() + " buf cleared (" +
                                   std::to_string(buf.size()) + " bytes)");
  buf.clear();
  bufEmptyCv.notify_one();
}

void SimtelUeContext::aquireBuf(size_t size) {
  std::unique_lock lock(bufMtx);
  bufEmptyCv.wait(lock, [this] { return buf.empty(); });
  buf.resize(size);
  MessageHolder::instance().addMsg(toStr() + " buf aquired (" +
                                   std::to_string(buf.size()) + " bytes)");
}

bool SimtelUeContext::fillBuf(const common::binary_t &data) {
  if (buf.size() != data.size()) {
    return false;
  }
  buf.assign(data.begin(), data.end());
  MessageHolder::instance().addMsg(toStr() + " buf filled (" +
                                   std::to_string(buf.size()) + " bytes)");
  return true;
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
  if (error) {
    return error;
  } else {
    buf.clear();
    return std::nullopt;
  }
}

std::string SimtelUeContext::toStr() const {
  return "UE_" + sock->getAddrStr();
}

bool SimtelUeContext::setReceiveTimeout(unsigned int timeoutMsec) {
  return sock->setReceiveTimeout(timeoutMsec);
}

bool SimtelUeContext::removeReceiveTimeout() {
  return sock->removeReceiveTimeout();
}
} // namespace server
