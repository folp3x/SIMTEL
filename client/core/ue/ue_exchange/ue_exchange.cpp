#include "ue_exchange.h"

namespace client {
UeExchange::UeExchange(const common::NetworkAddress &serverAddr_)
    : serverAddr(serverAddr_) {}

std::optional<std::string> UeExchange::updateConnection(bool ueActive) {
  if (ueActive) {
    return sock.connectTo(serverAddr);
  } else {
    sock.closeSock();
    return std::nullopt;
  }
}
} // namespace client
