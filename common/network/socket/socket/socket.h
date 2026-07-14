#pragma once

#include <expected>
#include <netinet/in.h>
#include <optional>

#include "common/constants.h"
#include "common/network/network_address/network_address.h"
#include "common/network/network_error/network_error/network_error.h"
#include "common/types.h"

namespace common {
class Socket {
private:
  static constexpr uint32_t MaxMsgSize = constants::BytesInMb;
  static constexpr int InvalidSock = -1;

  std::optional<NetworkError> sendAll(const void *data, size_t size) const;

protected:
  int sock = InvalidSock;

  Socket() = default;
  Socket(int sock_);

  static std::string getLastError();

  static sockaddr_in toSockAddr(const NetworkAddress &address);

  static std::expected<int, std::string> initSock();

public:
  virtual ~Socket();

  Socket(const Socket &) = delete;
  Socket &operator=(const Socket &) = delete;
  Socket(Socket &&other);
  Socket &operator=(Socket &&other);

  static bool setSendTimeout(int sock, unsigned int timeoutMsec);
  static bool setReceiveTimeout(int sock, unsigned int timeoutSec);

  void closeSock();

  std::optional<NetworkError> sendMessage(const binary_t &data) const;
  std::expected<binary_t, NetworkError> receiveMessage() const;
};
} // namespace common
