#include "socket.h"

#include <system_error>
#include <unistd.h>
#include <zpp_bits.h>

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

sockaddr_in Socket::toSockAddr(const NetworkAddress &address) {
  sockaddr_in sockAddr;
  sockAddr.sin_family = PF_INET;
  sockAddr.sin_port = htons(address.getPort());
  sockAddr.sin_addr.s_addr = htonl(address.getIP());

  return sockAddr;
}

Socket::Socket(int sock_) : sock(sock_) {}

Socket::Socket(Socket &&other) {
  sock = other.sock;
  other.sock = INVALID_SOCK;
}

Socket &Socket::operator=(Socket &&other) {
  if (&other != this) {
    sock = other.sock;
    other.sock = INVALID_SOCK;
  }

  return *this;
}

Socket::~Socket() { closeSock(); }

std::expected<int, std::string> Socket::initSock() {
  int inited = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (inited < 0) {
    return std::unexpected(getLastError());
  }

  timeval tv = {SEND_TIMEOUT_SEC, 0};
  if (setsockopt(inited, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
    close(inited);
    return std::unexpected(getLastError());
  }

  return inited;
}

void Socket::closeSock() {
  shutdown(sock, SHUT_RDWR);
  close(sock);
  sock = INVALID_SOCK;
}

std::optional<std::string> Socket::sendMessage(const binary_t &data) const {
  if (data.empty()) {
    return "Empty message";
  }

  if (data.size() > MAX_MSG_SIZE) {
    return "Too large message " +
           std::to_string(MAX_MSG_SIZE / constants::BYTES_IN_MB) + " MB";
  }

  auto error = sendAll(data.data(), data.size());
  if (error) {
    return error;
  }

  return std::nullopt;
}

std::expected<binary_t, std::string> Socket::receiveMessage() const {
  binary_t header;
  header.resize(constants::SOCKET_MESSAGE_HEADER_BYTES);

  // чтение заголовка
  ssize_t received =
      recv(sock, header.data(), header.size(), MSG_WAITALL | MSG_NOSIGNAL);
  if (received < 0)
    return std::unexpected(getLastError());
  if (received == 0)
    return std::unexpected("Connection closed");
  if (received != header.size())
    return std::unexpected("Incomplete header");

  auto in = zpp::bits::in(header);
  uint32_t msgSize = 0;
  if (in(msgSize) != zpp::bits::errc{}) {
    return std::unexpected("Failed to read size");
  }
  msgSize = ntohl(msgSize);

  // чтение данных
  binary_t content(msgSize);
  received = recv(sock, content.data(), msgSize, MSG_WAITALL | MSG_NOSIGNAL);
  if (received < 0)
    return std::unexpected(getLastError());
  if (received != msgSize)
    return std::unexpected("Incomplete content");

  binary_t result;
  result.reserve(header.size() + content.size());
  result.insert(result.end(), header.begin(), header.end());
  result.insert(result.end(), content.begin(), content.end());

  return result;
}
} // namespace common
