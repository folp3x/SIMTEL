#pragma once

#include <string>

#include <common/types/types.h>

namespace common {
class Validator {
private:
  // диапазон динамических (приватных) портов по спецификации IANA
  static constexpr int MIN_AVAILABLE_PORT = 49152;
  static constexpr int MAX_AVAILABLE_PORT = 65535;

  // диапазон значений младшего байта IP-адреса
  static constexpr int MIN_IP_LOW_BYTE = 1;
  static constexpr int MAX_IP_LOW_BYTE = 253;

  // 3 цифры MCC, минимум 2 цифры MNC и минимум 1 цифра MSIN
  static constexpr int MIN_IMSI_LENGTH = 6;

  static constexpr int MAX_IMEI_LENGTH = 15;
  static constexpr int MAX_IMSI_LENGTH = 15;

  static bool isCorrectJsonPath(std::string_view filePath);

public:
  static std::string isCorrectIP(uint32_t ip);
  static std::string isCorrectIpStr(const std::string &ipStr);
  static std::string isCorrectPort(int port);
  static std::string isCorrectPortStr(const std::string &portStr);
  static std::string isCorrectIMEI(const common::imei_t &imei);
  static std::string isCorrectIMSI(const common::imsi_t &imsi);
  static std::string isCorrectConfigPath(const std::string &filePath);
  static std::string isCorrectNodesPath(const std::string &filePath);
};
} // namespace common
