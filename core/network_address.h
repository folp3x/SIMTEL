#pragma once

#include <cstdint>
#include <string>

class NetworkAddress {
private:
  uint32_t ip;
  uint16_t port;

  void setIP(const std::string &ipStr);
  void setPort(const std::string &portStr);

public:
  NetworkAddress(uint32_t ip_, uint16_t port_);
  explicit NetworkAddress(const std::string &fullAddress);
  NetworkAddress(const std::string &ipAddress, const std::string &port_);
  NetworkAddress(const std::string &ipAddress, int port_);

  static NetworkAddress fromStr(const std::string &fullAddress);

  uint32_t getIP() const;

  uint16_t getPort() const;
};
