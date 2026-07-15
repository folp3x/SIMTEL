#include "network.h"

#include <arpa/inet.h>
#include <cmath>

#include "common/utils/str/str.h"
#include "common/validator/validator.h"

namespace common::utils {
// парсит IP, возвращает его в хостовом порядке байт
std::expected<uint64_t, std::string> parseIp(const std::string &str) {
  in_addr tempAddr{};
  if (inet_pton(PF_INET, str.c_str(), &(tempAddr)) != 1) {
    return std::unexpected("IP address parse error");
  }

  uint32_t ip = tempAddr.s_addr;

  std::string validationInfo = Validator::isCorrectIp(ip);
  if (validationInfo.empty()) {
    return ntohl(ip);
  }

  return std::unexpected(validationInfo);
}

std::expected<uint16_t, std::string> parsePort(const std::string &str) {
  auto portParseResult = fromString<float>(str);

  if (portParseResult) {
    float port = *portParseResult;
    if (port != std::floor(port)) {
      return std::unexpected("Port must be integer");
    }

    std::string error = Validator::isCorrectPort(port);
    if (!error.empty()) {
      return std::unexpected(error);
    }

    return port;
  } else {
    return std::unexpected("Port parse error: " + portParseResult.error());
  }
}
} // namespace common::utils

#include "network_impl.h"
