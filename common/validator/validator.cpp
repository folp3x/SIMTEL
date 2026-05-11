#include "validator.h"

#include <cstring>
#include <filesystem>
#include <optional>

#include "common/utils/network/network.h"
#include "common/utils/str/str.h"

namespace common {
// проверяет что путь является путем к файлу JSON
bool Validator::isCorrectJsonPath(std::string_view filePath) {
  int jsonExtLen = std::strlen(".json");
  return (filePath.size() < jsonExtLen) ||
         filePath.substr(filePath.size() - jsonExtLen) == ".json";
}

std::string Validator::jsonFilePathExists(const std::string &filePath,
                                          const std::string &name) {
  if (!isCorrectJsonPath(filePath)) {
    return name + " path must be a path to .json file";
  } else if (!std::filesystem::exists(filePath)) {
    return name + " file not found";
  }
  return "";
}

std::string Validator::isCorrectDigitStr(std::string_view str,
                                         std::optional<int> minLength_,
                                         std::optional<int> maxLength_,
                                         const std::string &name) {
  int minLength, maxLength;
  std::string minLengthStr, maxLengthStr;
  bool lessDigits, moreDigits;
  if (minLength_) {
    minLength = *minLength_;
    if (minLength < 0)
      throw std::invalid_argument("minLength_ must be > 0");

    minLengthStr = std::to_string(minLength);
    lessDigits = str.size() < minLength;
  }

  if (maxLength_) {
    maxLength = *minLength_;
    if (maxLength < 0)
      throw std::invalid_argument("maxLength_ must be > 0");

    minLengthStr = std::to_string(maxLength);
    moreDigits = str.size() < minLength;
  }

  if (minLength && maxLength && (lessDigits || moreDigits)) {
    return name + " must have from " + minLengthStr + " to " + maxLengthStr +
           " digits";
  }
  if (minLength && lessDigits)
    return name + " must have more than " + minLengthStr + " digits";
  if (maxLength && moreDigits)
    return name + " must have less than " + minLengthStr + " digits";
  if (!allDigits(str))
    return name + " must contain only digits";

  return "";
}

// проверяет коррекность IPv4. ip должен иметь сетевой порядок байт
std::string Validator::isCorrectIP(uint32_t ip) {
  uint8_t lowByte = (ip >> 3 * 8) & 0xFF;
  if (lowByte < MIN_IP_LOW_BYTE || lowByte > MAX_IP_LOW_BYTE) {
    return "IP low byte must be from " + std::to_string(MIN_IP_LOW_BYTE) +
           " to " + std::to_string(MAX_IP_LOW_BYTE);
  }
  return "";
}

std::string Validator::isCorrectIpStr(const std::string &ipStr) {
  auto parseResult = parseIP(ipStr);
  if (parseResult) {
    return "";
  }
  return parseResult.error();
}

std::string Validator::isCorrectPort(int port) {
  if (port < MIN_AVAILABLE_PORT || port > MAX_AVAILABLE_PORT) {
    return "Port must be from " + std::to_string(MIN_AVAILABLE_PORT) + " to " +
           std::to_string(MAX_AVAILABLE_PORT);
  }
  return "";
}

std::string Validator::isCorrectPortStr(const std::string &portStr) {
  auto parseResult = parsePort(portStr);
  if (parseResult) {
    return "";
  }
  return parseResult.error();
}

std::string Validator::isCorrectIMEI(const common::imei_t &imei) {
  return isCorrectDigitStr(imei, MIN_IMEI_LENGTH, MAX_IMEI_LENGTH, "IMEI");
}

std::string Validator::isCorrectIMSI(const common::imsi_t &imsi) {
  return isCorrectDigitStr(imsi, MIN_IMSI_LENGTH, MAX_IMSI_LENGTH, "IMSI");
}

std::string Validator::isCorrectConfigPath(const std::string &filePath) {
  return jsonFilePathExists(filePath, "Config");
}

std::string Validator::isCorrectNodesPath(const std::string &filePath) {
  return jsonFilePathExists(filePath, "Nodes");
}
} // namespace common
