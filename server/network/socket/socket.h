#pragma once

#include "common/network/socket/socket/socket.h"

#include <memory>

namespace server {
class Socket : public common::Socket {
private:
  static constexpr int MAX_WAITING_CONNECTIONS = 10;
  static constexpr int SEND_TIMEOUT_SEC = 10;

  sockaddr_in sockAddr;

public:
  Socket() = default;
  Socket(int sock_, const sockaddr_in &sockAddr_);

  static std::expected<std::unique_ptr<Socket>, std::string>
  create(const common::NetworkAddress &address);

  std::optional<std::string> listenForConnections() const;
  std::expected<std::unique_ptr<Socket>, std::string> acceptConnection() const;

  std::string getAddrStr() const;

  bool setReceiveTimeout(unsigned int timeoutSec);
  bool removeReceiveTimeout();
};
} // namespace server
