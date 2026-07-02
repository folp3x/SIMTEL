#pragma once

#include <cstdint>
#include <string>

namespace common {
class NetworkAddress {
private:
  uint32_t ip;
  uint16_t port;

  void setIp(const std::string &ipStr);
  void setPort(const std::string &portStr);

public:
  NetworkAddress(uint32_t ip_, uint16_t port_);
  explicit NetworkAddress(const std::string &fullAddress);
  NetworkAddress(const std::string &ipAddress, const std::string &port_);
  NetworkAddress(const std::string &ipAddress, int port_);

  static NetworkAddress fromStr(const std::string &fullAddress);

  std::string toStr() const;

  uint32_t getIp() const;

  uint16_t getPort() const;
};
} // namespace common
