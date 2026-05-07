#pragma once

#include <string>

namespace common {
class Validator {
private:
  // диапазон динамических (приватных) портов по спецификации IANA
  static const int MIN_AVAILABLE_PORT = 49152;
  static const int MAX_AVAILABLE_PORT = 65535;

  // диапазон значений младшего байта IP-адреса
  static const int MIN_IP_LOW_BYTE = 1;
  static const int MAX_IP_LOW_BYTE = 253;

  static const int IMEI_LENGTH = 15;
  static const int IMSI_LENGTH = 15;

  static bool isCorrectJsonPath(const std::string &filePath);

public:
  static std::string isCorrectIP(uint32_t ip);
  static std::string isCorrectIpStr(const std::string &ipStr);
  static std::string isCorrectPort(int port);
  static std::string isCorrectPortStr(const std::string &portStr);
  static std::string isCorrectIMEI(const std::string &imei);
  static std::string isCorrectIMSI(const std::string &imsi);
  static std::string isCorrectConfigPath(const std::string &filePath);
  static std::string isCorrectNodesPath(const std::string &filePath);
};
} // namespace common
