#include "network_address.h"

#include <arpa/inet.h>
#include <stdexcept>

#include "common/utils/network/network.h"

namespace common {
void NetworkAddress::setIp(const std::string &ipStr) {
  auto parseResult = parseIp(ipStr);
  if (!parseResult) {
    throw std::invalid_argument(parseResult.error());
  }
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

NetworkAddress::NetworkAddress(const std::string &fullAddress)
    : NetworkAddress(fromStr(fullAddress)) {}

NetworkAddress::NetworkAddress(const std::string &ipAddress,
                               const std::string &port_) {
  setIp(ipAddress);
  setPort(port_);
}

NetworkAddress::NetworkAddress(const std::string &ipAddress, int port_) {
  setIp(ipAddress);
  port = port_;
}

std::string NetworkAddress::toStr() const {
  char ipStr[INET_ADDRSTRLEN];
  in_addr tempAddr;
  tempAddr.s_addr = htonl(ip);

  inet_ntop(AF_INET, &tempAddr, ipStr, sizeof(ipStr));

  return std::string(ipStr) + ":" + std::to_string(port);
}

NetworkAddress NetworkAddress::fromStr(const std::string &fullAddress) {
  size_t delim = fullAddress.rfind(':');
  if (delim == std::string::npos) {
    throw std::invalid_argument("fullAddress must have port delimeter");
  }
  std::string ipAddress = fullAddress.substr(0, delim);
  std::string port = fullAddress.substr(delim + 1);

  return NetworkAddress{ipAddress, port};
}

uint32_t NetworkAddress::getIp() const { return ip; }

uint16_t NetworkAddress::getPort() const { return port; }
} // namespace common
