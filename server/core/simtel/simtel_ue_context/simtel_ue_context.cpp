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

std::weak_ptr<SimtelBaseStation> SimtelUeContext::getBs() const { return bs; }

void SimtelUeContext::setBs(std::weak_ptr<SimtelBaseStation> bs_) { bs = bs_; }

common::Protocol SimtelUeContext::getProtocol() const { return protocol; }

void SimtelUeContext::setProtocol(common::Protocol protocol_) {
  protocol = protocol_;
}

common::binary_t SimtelUeContext::takeBuf() {
  common::binary_t copy = buf;
  buf.clear();

  return copy;
}

void SimtelUeContext::setBuf(const common::binary_t &buf_) {
  buf = buf_;
  MessageHolder::instance().addMsg(toStr() + " buf set (" +
                                   std::to_string(buf.size()) + " bytes)");
}

common::binary_t SimtelUeContext::copyBuf() const {
  MessageHolder::instance().addMsg(toStr() + " buf copied (" +
                                   std::to_string(buf.size()) + " bytes)");
  return buf;
}

void SimtelUeContext::clearBuf() {
  buf.clear();
  MessageHolder::instance().addMsg(toStr() + " buf cleared");
}

void SimtelUeContext::aquireBuf(size_t size) {
  buf.resize(size);
  MessageHolder::instance().addMsg(toStr() + " buf aquired (" +
                                   std::to_string(buf.size()) + " bytes)");
}

bool SimtelUeContext::fillBuf(const common::binary_t &data) {
  if (buf.size() != data.size()) {
    return false;
  }

  buf.assign(data.begin(), data.end());
  MessageHolder::instance().addMsg(toStr() + " buf filled");
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
    return *error;
  } else {
    clearBuf();
    return std::nullopt;
  }
}

std::string SimtelUeContext::toStr() const {
  std::string mTimsiStr = mTimsi.empty() ? "?" : mTimsi;
  return "UE_" + sock->getAddrStr() + "(m-timsi=" + mTimsiStr + ")";
}

bool SimtelUeContext::setReceiveTimeout(unsigned int timeoutMsec) {
  return sock->setReceiveTimeout(timeoutMsec);
}

bool SimtelUeContext::removeReceiveTimeout() {
  return sock->removeReceiveTimeout();
}

std::shared_ptr<std::mutex> SimtelUeContext::getSendMtx() const {
  return sendMtx;
}
} // namespace server
