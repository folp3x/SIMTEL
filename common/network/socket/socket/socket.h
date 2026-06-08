#pragma once

#include <expected>
#include <netinet/in.h>
#include <optional>
#include <sys/socket.h>

#include "common/constants.h"
#include "common/network/network_address/network_address.h"
#include "common/types.h"

namespace common {
class Socket {
private:
  static constexpr size_t MAX_MSG_SIZE = constants::BYTES_IN_MB;
  static constexpr size_t MSG_PARTS_COUNT = 2;
  static constexpr int INVALID_SOCK = -1;
  static constexpr int SEND_TIMEOUT_SEC = 10;

  std::optional<std::string> sendAll(const void *data, size_t size_) const;

protected:
  int sock = INVALID_SOCK;

  static std::string getLastError();

  static sockaddr_in toSockAddr(const common::NetworkAddress &address);

  static std::expected<int, std::string> initSock();

public:
  Socket() = default;
  Socket(int sock_);
  ~Socket();

  void closeSock();

  Socket(const Socket &) = delete;
  Socket &operator=(const Socket &) = delete;
  Socket(Socket &&other);
  Socket &operator=(Socket &&other);

  std::optional<std::string> sendMessage(const binary_t &data) const;
  std::expected<binary_t, std::string> receiveMessage() const;
};
} // namespace common
