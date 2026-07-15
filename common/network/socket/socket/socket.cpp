#include "socket.h"

#include <system_error>
#include <unistd.h>
#include <zpp_bits.h>

#include "common/network/socket/socket_message_header/socket_message_header.h"
#include "common/utils/network/network.h"

namespace common {
std::optional<NetworkError> Socket::sendAll(const void *data,
                                            size_t size) const {
  const char *ptr = static_cast<const char *>(data);
  size_t leftSize = size;

  while (leftSize > 0) {
    ssize_t sent = send(sock, ptr, leftSize, MSG_NOSIGNAL);
    if (sent < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return NetworkError{NetworkErrorType::SendTimeout, "Send timeout"};
      }
      return NetworkError{NetworkErrorType::Other, getLastError()};
    } else if (sent == 0) {
      return NetworkError{NetworkErrorType::ConnectionClosed,
                          "Connection closed"};
    }

    ptr += sent;
    leftSize -= sent;
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
  sockAddr.sin_addr.s_addr = htonl(address.getIp());

  return sockAddr;
}

Socket::Socket(int sock_) : sock(sock_) {}

Socket::Socket(Socket &&other) {
  sock = other.sock;
  other.sock = InvalidSock;
}

Socket &Socket::operator=(Socket &&other) {
  if (&other != this) {
    sock = other.sock;
    other.sock = InvalidSock;
  }
  return *this;
}

Socket::~Socket() { closeSock(); }

std::expected<int, std::string> Socket::initSock() {
  int inited = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (inited < 0) {
    return std::unexpected(getLastError());
  }
  return inited;
}

void Socket::closeSock() {
  shutdown(sock, SHUT_RDWR);
  close(sock);
  sock = InvalidSock;
}

bool Socket::setSendTimeout(int sock, unsigned int timeoutSec) {
  timeval tv = {timeoutSec, 0};
  return setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) == 0;
}

bool Socket::setReceiveTimeout(int sock, unsigned int timeoutMsec) {
  timeval tv;
  tv.tv_sec = timeoutMsec / 1000;
  tv.tv_usec = (timeoutMsec % 1000) * 1000;
  return setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == 0;
}

std::optional<NetworkError> Socket::sendMessage(const binary_t &data) const {
  if (data.empty()) {
    return NetworkError{NetworkErrorType::EmptyMessage, "Empty message"};
  }

  if (data.size() > MaxMsgBytes) {
    return NetworkError{NetworkErrorType::LargeMessage,
                        "Message cant be larger than " +
                            std::to_string(MaxMsgBytes / constants::BytesInMb) +
                            " MB"};
  }

  auto error = sendAll(data.data(), data.size());
  if (error) {
    return error;
  }

  return std::nullopt;
}

std::expected<binary_t, NetworkError> Socket::receiveMessage() const {
  binary_t header;
  header.resize(constants::SocketMessageHeaderBytes);

  // чтение заголовка
  ssize_t received =
      recv(sock, header.data(), header.size(), MSG_WAITALL | MSG_NOSIGNAL);
  if (received == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return std::unexpected(
          NetworkError{NetworkErrorType::ReceiveTimeout, "Receive timeout"});
    }
    if (errno == ECONNRESET) {
      return std::unexpected(NetworkError{NetworkErrorType::ConnectionClosed,
                                          "Connection closed"});
    }
    if (errno == EBADF) {
      return std::unexpected(NetworkError{NetworkErrorType::BadFileDescriptor,
                                          "Bad file descriptor"});
    }
    return std::unexpected(
        NetworkError{NetworkErrorType::Other, getLastError()});
  } else if (received == 0) {
    return std::unexpected(
        NetworkError{NetworkErrorType::ConnectionClosed, "Connection closed"});
  } else if (received != header.size()) {
    return std::unexpected(
        NetworkError{NetworkErrorType::IncompleteHeader, "Incomplete header"});
  }

  auto in = zpp::bits::in(header, zpp::bits::endian::big{});
  uint32_t msgSize = 0;
  if (zpp::bits::failure(in(msgSize))) {
    return std::unexpected(
        NetworkError{NetworkErrorType::NoMsgSize, "Failed to read size"});
  }

  if (msgSize == 0) {
    return header;
  }

  // чтение данных
  binary_t content(msgSize);
  received = recv(sock, content.data(), msgSize, MSG_WAITALL | MSG_NOSIGNAL);
  if (received == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return std::unexpected(
          NetworkError{NetworkErrorType::ReceiveTimeout, "Receive timeout"});
    }
    if (errno == ECONNRESET) {
      return std::unexpected(NetworkError{NetworkErrorType::ConnectionClosed,
                                          "Connection closed"});
    }
    if (errno == EBADF) {
      return std::unexpected(NetworkError{NetworkErrorType::BadFileDescriptor,
                                          "Bad file descriptor"});
    }
    return std::unexpected(
        NetworkError{NetworkErrorType::Other, getLastError()});
  } else if (received == 0) {
    return std::unexpected(
        NetworkError{NetworkErrorType::ConnectionClosed, "Connection closed"});
  } else if (received != msgSize) {
    std::string description = "Incomplete content: received " +
                              std::to_string(received) + ", expected " +
                              std::to_string(msgSize);
    return std::unexpected(
        NetworkError{NetworkErrorType::IncompleteContent, description});
  }

  return utils::mergeBinary(header, content);
}
} // namespace common
