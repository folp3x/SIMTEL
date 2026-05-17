#pragma once

#include "common/core/location/location/location.h"
#include "common/network/protocol/protocol.h"
#include "common/network/socket/socket/socket.h"

namespace server {
class Socket : public common::Socket {
private:
  static constexpr int MAX_WAITING_CONNECTIONS = 10;

  sockaddr_in sockAddr;

  void logReceiveLocation(const std::string &dataStr) const;
  void logSendDistance(const std::string &dataStr) const;

public:
  Socket() = default;
  Socket(int sock_, const sockaddr_in &sockAddr_);

  static std::expected<std::unique_ptr<Socket>, std::string>
  create(const common::NetworkAddress &address);

  std::optional<std::string> listenForConnections() const;
  std::expected<std::unique_ptr<Socket>, std::string> acceptConnection() const;

  std::expected<common::Location<>, std::string>
  receiveLocation(common::Protocol &clientProtocol) const;
  std::optional<std::string> sendDistance(common::Protocol protocol,
                                          float distance) const;

  std::string getAddrStr() const;
};
} // namespace server
