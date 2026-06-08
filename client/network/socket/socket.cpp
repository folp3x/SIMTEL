#include "socket.h"

#include <unistd.h>

namespace client {
std::expected<int, std::string> Socket::initSock() {
  auto initResult = common::Socket::initSock();
  if (!initResult) {
    return initResult;
  }

  int inited = *initResult;
  timeval tv = {RECEIVE_TIMEOUT_SEC, 0};
  if (setsockopt(inited, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    close(inited);
    return std::unexpected(getLastError());
  }

  return inited;
}

std::optional<std::string>
Socket::connectTo(const common::NetworkAddress &address) {
  auto initResult = initSock();
  if (!initResult) {
    return initResult.error();
  }

  sock = *initResult;

  sockaddr_in serverAddr = common::Socket::toSockAddr(address);
  if (connect(sock, reinterpret_cast<sockaddr *>(&serverAddr),
              sizeof(serverAddr)) < 0) {
    return common::Socket::getLastError();
  }

  return std::nullopt;
}
} // namespace client
