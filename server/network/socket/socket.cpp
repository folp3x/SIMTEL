#include "socket.h"

#include <arpa/inet.h>

namespace server {
Socket::Socket(int sock_, const sockaddr_in &sockAddr_)
    : common::Socket(sock_), sockAddr(sockAddr_) {}

std::expected<std::unique_ptr<Socket>, std::string>
Socket::create(const common::NetworkAddress &address) {
  auto initResult = initSock();
  if (!initResult) {
    return std::unexpected(initResult.error());
  }

  int sock = *initResult;
  sockaddr_in sockAddr = toSockAddr(address);

  bool sendTimeoutSet = setSendTimeout(sock, SEND_TIMEOUT_SEC);
  if (!sendTimeoutSet) {
    return std::unexpected("Error setting send timeout");
  }

  if (bind(sock, reinterpret_cast<sockaddr *>(&sockAddr), sizeof(sockAddr)) <
      0) {
    return std::unexpected(getLastError());
  }

  return std::make_unique<Socket>(sock, sockAddr);
}

std::optional<std::string> Socket::listenForConnections() const {
  if (listen(sock, MAX_WAITING_CONNECTIONS) < 0) {
    return getLastError();
  }
  return std::nullopt;
}

std::expected<std::unique_ptr<Socket>, std::string>
Socket::acceptConnection() const {
  sockaddr_in clientAddr;
  socklen_t addrLen = sizeof(clientAddr);

  int clientSock =
      accept(sock, reinterpret_cast<sockaddr *>(&clientAddr), &addrLen);
  if (clientSock < 0) {
    return std::unexpected(getLastError());
  }

  return std::make_unique<Socket>(clientSock, clientAddr);
}

std::string Socket::getAddrStr() const {
  char buf[INET_ADDRSTRLEN];

  if (!inet_ntop(AF_INET, &sockAddr.sin_addr, buf, INET_ADDRSTRLEN)) {
    return "invalid addr";
  }

  return std::string(buf) + ":" + std::to_string(ntohs(sockAddr.sin_port));
}

bool Socket::setReceiveTimeout(unsigned int timeoutMsec) {
  return common::Socket::setReceiveTimeout(sock, timeoutMsec);
}

bool Socket::removeReceiveTimeout() {
  return common::Socket::setReceiveTimeout(sock, 0);
}
} // namespace server
