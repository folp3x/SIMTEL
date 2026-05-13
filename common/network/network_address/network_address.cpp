#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include "network_address.h"

#include <arpa/inet.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

#include "common/utils/network/network.h"

namespace common {
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

NetworkAddress::NetworkAddress(const NetworkAddress &other)
    : ip(other.ip), port(other.port) {
  logOperation("COPY constructor", ip, port);
}

NetworkAddress &NetworkAddress::operator=(const NetworkAddress &other) {
  if (&other != this) {
    this->ip = other.ip;
    this->port = other.port;
  }

  logOperation("COPY operator", ip, port);

  return *this;
}

NetworkAddress::NetworkAddress(NetworkAddress &&other) noexcept
    : ip(other.ip), port(other.port) {
  logOperation("MOVE constructor", ip, port);
}

NetworkAddress &NetworkAddress::operator=(NetworkAddress &&other) noexcept {
  if (&other != this) {
    this->ip = other.ip;
    this->port = other.port;
  }

  logOperation("MOVE operator", ip, port);

  return *this;
}

void NetworkAddress::logOperation(const std::string &operationName, uint32_t ip,
                                  uint16_t port) const {
  SPDLOG_LOGGER_DEBUG(spdlog::default_logger(),
                      "common::NetworkAddress {} called: ip={:#x}, port={}",
                      operationName, ip, port);
}

std::string NetworkAddress::toStr() const {
  char ipStr[INET_ADDRSTRLEN];
  struct in_addr tempAddr;
  tempAddr.s_addr = ip;

  inet_ntop(AF_INET, &tempAddr, ipStr, sizeof(ipStr));

  return std::string(ipStr) + ":" + std::to_string(port);
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
} // namespace common
