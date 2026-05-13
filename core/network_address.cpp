#include "network_address.h"

#include <stdexcept>

#include "utils/network.h"

void NetworkAddress::setIP(const std::string &ipStr) {
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

NetworkAddress::NetworkAddress(const std::string &fullAddress)
    : NetworkAddress(fromStr(fullAddress)) {}

NetworkAddress::NetworkAddress(const std::string &ipAddress,
                               const std::string &port_) {
  setIP(ipAddress);
  setPort(port_);
}

NetworkAddress::NetworkAddress(const std::string &ipAddress, int port_) {
  setIP(ipAddress);
  port = port_;
}

// парсинг объекта из строки с полным адресом
NetworkAddress NetworkAddress::fromStr(const std::string &fullAddress) {
  size_t delim = fullAddress.rfind(":");
  if (delim == std::string::npos)
    throw std::invalid_argument("fullAddress must have port delimeter");

  std::string ipAddress = fullAddress.substr(0, delim);
  std::string port = fullAddress.substr(delim + 1);

  return NetworkAddress{ipAddress, port};
}

uint32_t NetworkAddress::getIP() const { return ip; }

uint16_t NetworkAddress::getPort() const { return port; }
