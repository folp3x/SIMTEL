#include "socket.h"

#include <system_error>
#include <unistd.h>

#include "common/network/socket/socket_message_header/socket_message_header.h"

namespace common {
std::optional<std::string> Socket::sendAll(const void *data,
                                           size_t size_) const {
  const char *ptr = static_cast<const char *>(data);
  size_t size = size_;

  while (size > 0) {
    ssize_t sent = send(sock, ptr, size, MSG_NOSIGNAL);
    if (sent < 0)
      return getLastError();
    if (sent == 0)
      return "Connection closed";

    ptr += sent;
    size -= sent;
  }

  return std::nullopt;
}

std::string Socket::getLastError() {
  std::error_code error(errno, std::generic_category());
  return error.message();
}

sockaddr_in Socket::toSockAddr(const common::NetworkAddress &address) {
  sockaddr_in sockAddr;
  sockAddr.sin_family = PF_INET;
  sockAddr.sin_port = htons(address.getPort());
  sockAddr.sin_addr.s_addr = htonl(address.getIP());

  return sockAddr;
}

Socket::Socket(int sock_) : sock(sock_) {}

Socket::~Socket() { closeSock(); }

std::expected<int, std::string> Socket::initSock() {
  int inited = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (inited < 0) {
    return std::unexpected(getLastError());
  }

  timeval tv = {5, 0}; // таймаут 5 секунд на отправку
  if (setsockopt(inited, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
    close(inited);
    return std::unexpected(getLastError());
  }

  return inited;
}

void Socket::closeSock() const {
  shutdown(sock, SHUT_RDWR);
  close(sock);
}

std::optional<std::string> Socket::sendMessage(uint8_t protocol,
                                               const binary_t &content) const {
  if (content.empty()) {
    return "Empty message";
  }

  if (content.size() > MAX_MSG_SIZE) {
    return "Too large message " +
           std::to_string(MAX_MSG_SIZE / constants::BYTES_IN_MB) + " MB";
  }

  SocketMessageHeader header{protocol, htonl(content.size())};

  // отправка заголовка
  auto error = sendAll(&header, sizeof(header));
  if (error) {
    return error;
  }

  // отправка данных
  error = sendAll(content.data(), content.size());
  if (error) {
    return error;
  }

  return std::nullopt;
}

std::expected<SocketMessage, std::string> Socket::receiveMessage() const {
  SocketMessageHeader header;

  // чтение заголовка
  ssize_t received =
      recv(sock, &header, sizeof(header), MSG_WAITALL | MSG_NOSIGNAL);
  if (received < 0)
    return std::unexpected(getLastError());
  if (received == 0)
    return std::unexpected("Connection closed");
  if (received != sizeof(header))
    return std::unexpected("Incomplete header");

  uint32_t msgSize = ntohl(header.msgSize);
  if (msgSize > MAX_MSG_SIZE) {
    return std::unexpected("Too large message");
  }

  // чтение данных
  binary_t data(msgSize);
  received = recv(sock, data.data(), msgSize, MSG_WAITALL | MSG_NOSIGNAL);
  if (received < 0)
    return std::unexpected(getLastError());
  if (received != msgSize)
    return std::unexpected("Incomplete data");

  return SocketMessage{header.protocol, std::move(data)};
}
} // namespace common
