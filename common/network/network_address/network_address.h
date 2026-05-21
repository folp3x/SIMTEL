#pragma once

#include <cstdint>
#include <string>

namespace common {
class NetworkAddress {
private:
  uint32_t ip;
  uint16_t port;

  void setIP(const std::string &ipStr);
  void setPort(const std::string &portStr);

  void logOperation(const std::string &operationName, uint32_t ip,
                    uint16_t port) const;

public:
  NetworkAddress(uint32_t ip_, uint16_t port_);
  explicit NetworkAddress(const std::string &fullAddress);
  NetworkAddress(const std::string &ipAddress, const std::string &port_);
  NetworkAddress(const std::string &ipAddress, int port_);

  NetworkAddress(const NetworkAddress &other);
  NetworkAddress &operator=(const NetworkAddress &other);
  NetworkAddress(NetworkAddress &&other) noexcept;
  NetworkAddress &operator=(NetworkAddress &&other) noexcept;

  static NetworkAddress fromStr(const std::string &fullAddress);

  std::string toStr() const;

  uint32_t getIP() const;

  uint16_t getPort() const;
};
} // namespace common
