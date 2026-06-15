#include "socket.h"

#include <unistd.h>

namespace client {
std::optional<std::string>
Socket::connectTo(const common::NetworkAddress &address) {
  auto initResult = common::Socket::initSock();
  if (!initResult) {
    return initResult.error();
  }

  sock = *initResult;

  bool sendTimeoutSet = setSendTimeout(sock, SEND_TIMEOUT_SEC);
  if (!sendTimeoutSet) {
    return "Error setting send timeout";
  }

  sockaddr_in serverAddr = common::Socket::toSockAddr(address);
  if (connect(sock, reinterpret_cast<sockaddr *>(&serverAddr),
              sizeof(serverAddr)) < 0) {
    return common::Socket::getLastError();
  }

  return std::nullopt;
}

bool Socket::setReceiveTimeout(unsigned int timeoutMsec) {
  return common::Socket::setReceiveTimeout(sock, timeoutMsec);
}
} // namespace client
