#pragma once

#include <optional>

#include "common/types.h"

namespace common {
class Validator {
private:
  // диапазон динамических (приватных) портов по спецификации IANA
  static constexpr int MinAvailablePort = 49152;
  static constexpr int MaxAvaliablePort = 65535;

  // диапазон значений младшего байта IP-адреса
  static constexpr int MinIpLowByte = 1;
  static constexpr int MaxIpLowByte = 253;

  static constexpr size_t MinImeiLength = 1;
  static constexpr size_t MaxImeiLength = 15;

  // 3 цифры MCC, минимум 2 цифры MNC и минимум 1 цифра MSIN
  static constexpr size_t MimImsiLength = 6;
  static constexpr size_t MaxImsiLength = 15;

  static constexpr size_t MsisdnLength = 11;
  static constexpr char MsisdnAnyDigit = 'x';

  inline static const std::string MsisdnFormatStr =
      "8" + std::string(MsisdnLength, MsisdnAnyDigit);

  static bool isCorrectJsonPath(std::string_view filePath);

protected:
  static std::string isCorrectDigitStr(std::string_view str,
                                       std::optional<int> minLength_,
                                       std::optional<int> maxLength_,
                                       const std::string &name);

public:
  static std::string jsonFilePathExists(const std::string &filePath,
                                        const std::string &name);

  static std::string isCorrectIp(uint32_t ip);
  static std::string isCorrectIpStr(const std::string &ipStr);
  static std::string isCorrectPort(int port);
  static std::string isCorrectPortStr(const std::string &portStr);
  static std::string isCorrectImei(const imei_t &imei);
  static std::string isCorrectImsi(const imsi_t &imsi);
  static std::string isCorrectMsisdn(const msisdn_t &msisdn);
  static std::string
  isCorrectSpeedDialNumStr(const std::string &speedDialNumStr);
  static std::string isCorrectSignal(unsigned int signal);
  static std::string isPositiveNumber(double number, const std::string &name);
};
} // namespace common
