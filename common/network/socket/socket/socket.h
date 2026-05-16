#pragma once

#include <expected>
#include <netinet/in.h>
#include <optional>
#include <sys/socket.h>

#include "common/constants.h"
#include "common/network/network_address/network_address.h"
#include "common/network/socket/socket_message/socket_message.h"

namespace common {
class Socket {
private:
  static constexpr size_t MAX_MSG_SIZE = 1 * constants::BYTES_IN_MB;
  static constexpr size_t MSG_PARTS_COUNT = 2;
  static constexpr int INVALID_SOCK = -1;

  std::optional<std::string> sendAll(const void *data, size_t size_) const;

protected:
  int sock = INVALID_SOCK;

  static std::string getLastError();

  static sockaddr_in toSockAddr(const common::NetworkAddress &address);

  static std::expected<int, std::string> initSock();

  void closeSock() const;

public:
  Socket() = default;
  Socket(int sock_);
  ~Socket();

  Socket(const Socket &) = delete;
  Socket &operator=(const Socket &) = delete;

  std::optional<std::string> sendMessage(uint8_t protocol,
                                         const binary_t &content) const;
  std::expected<SocketMessage, std::string> receiveMessage() const;
};
} // namespace common
