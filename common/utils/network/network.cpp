#include "network.h"

#include <arpa/inet.h>
#include <cmath>
#include <stdexcept>

#include "common/utils/str/str.h"
#include "common/validator/validator.h"

namespace common {
// парсит IP, возвращает его в хостовом порядке байт
std::expected<uint64_t, std::string> parseIP(const std::string &str) {
  in_addr tempAddr{};
  if (inet_pton(PF_INET, str.c_str(), &(tempAddr)) != 1) {
    return std::unexpected("IP address parse error");   
  }

  uint32_t ip = tempAddr.s_addr;

  std::string validationInfo = Validator::isCorrectIP(ip);
  if (validationInfo.empty()) {
    return ntohl(ip);
  }
  return std::unexpected(validationInfo);
}

std::expected<uint16_t, std::string> parsePort(const std::string &str) {
  auto portParseResult = common::fromString<float>(str);

  if (portParseResult) {
    float port = *portParseResult;
    if (port != floor(port)) {
      return std::unexpected("Port must be integer");   
    }

    std::string validationInfo = Validator::isCorrectPort(port);
    if (validationInfo.empty()) {
      return port;
    }
    return std::unexpected(validationInfo);
  } else {
    return std::unexpected("Port parse error: " + portParseResult.error());
  }
}
} // namespace common
