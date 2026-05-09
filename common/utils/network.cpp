#include "network.h"

#include <arpa/inet.h>
#include <cmath>
#include <stdexcept>

#include "common/validator/validator.h"

namespace common {
std::expected<uint64_t, std::string> parseIP(const std::string &str) {
  in_addr tempAddr{};
  if (inet_pton(AF_INET, str.c_str(), &(tempAddr)) != 1)
    return std::unexpected("IP address parse error");

  in_addr_t tempIP = tempAddr.s_addr;
  uint32_t ip = static_cast<uint32_t>(tempIP);

  std::string validationInfo = Validator::isCorrectIP(ip);
  if (validationInfo.empty()) {
    return ip;
  }
  return std::unexpected(validationInfo);
}

std::expected<uint16_t, std::string> parsePort(const std::string &str) {
  try {
    double port = std::stod(str);
    if (port != std::floor(port))
      return std::unexpected("Port must be integer");

    std::string validationInfo = Validator::isCorrectPort(port);
    if (validationInfo.empty()) {
      return port;
    }
    return std::unexpected(validationInfo);
  } catch (const std::invalid_argument &) {
    return std::unexpected("Port must be numeric");
  } catch (const std::out_of_range &) {
    return std::unexpected("Port value out of range");
  }
}
} // namespace common
