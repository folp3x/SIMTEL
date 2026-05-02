#include "network_address.h"

#include <stdexcept>

#include "utils/network.h"

void NetworkAddress::setIp(const std::string &ipStr) {
  auto parseResult = parseIP(ipStr);
  if (!parseResult)
    throw std::invalid_argument(parseResult.error());
  ip = *parseResult;
}

void NetworkAddress::setPort(const std::string &portStr) {
  auto parseResult = parsePort(portStr);
  if (!parseResult) {
    throw std::invalid_argument(parseResult.error());
  }
  port = *parseResult;
}

NetworkAddress::NetworkAddress(uint32_t ip_, uint16_t port_)
    : ip(ip_), port(port_) {}

NetworkAddress::NetworkAddress(const std::string &fullAddress) {
  size_t delim = fullAddress.rfind(":");
  if (delim == std::string::npos)
    throw std::invalid_argument("fullAddress must have port delimeter");

  std::string ipStr = fullAddress.substr(0, delim);
  setIp(ipStr);

  std::string portStr = fullAddress.substr(delim + 1);
  setPort(portStr);
}

NetworkAddress::NetworkAddress(const std::string &ipAddress,
                               const std::string &port) {
  setIp(ipAddress);
  setPort(port);
}
