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
    double port = *portParseResult;
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

std::string toStr(const binary_t &binary) {
  if (binary.empty()) {
    return "";
  }

  constexpr int ONE_BYTE_CHARS = 3;
  std::string str;
  str.reserve(binary.size() * ONE_BYTE_CHARS);
  char buf[ONE_BYTE_CHARS];

  for (size_t i = 0; i < binary.size(); ++i) {
    if (i > 0) {
      str += " ";
    }
    snprintf(buf, sizeof(buf), "%02X", static_cast<uint8_t>(binary[i]));

    str += buf;
  }

  return str;
}
} // namespace common
