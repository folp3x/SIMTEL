#include "validator.h"

#include <cstring>
#include <filesystem>

#include "common/utils/network/network.h"
#include "common/utils/str/str.h"

namespace common {
bool Validator::isCorrectJsonPath(std::string_view filePath) {
  size_t jsonExtLen = std::strlen(".json");
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
    if (minLength < 0) {
      throw std::invalid_argument("minLength_ must be > 0");
    }

    minLengthStr = std::to_string(minLength);
    lessDigits = str.size() < minLength;
  }

  if (maxLength_) {
    maxLength = *maxLength_;
    if (maxLength < 0) {
      throw std::invalid_argument("maxLength_ must be > 0");
    }

    maxLengthStr = std::to_string(maxLength);
    moreDigits = str.size() > maxLength;
  }

  if (minLength && maxLength && (lessDigits || moreDigits)) {
    return name + " must have from " + minLengthStr + " to " + maxLengthStr +
           " digits";
  }
  if (minLength && lessDigits)
    return name + " must have more than " + minLengthStr + " digits";
  if (maxLength && moreDigits)
    return name + " must have less than " + minLengthStr + " digits";
  if (!utils::allDigits(str))
    return name + " must contain only digits";

  return "";
}

// проверяет коррекность IPv4. IP должен иметь хостовой порядок байт
std::string Validator::isCorrectIp(uint32_t ip) {
  uint8_t lowByte = ip & 0xFF;
  if (lowByte < MinIpLowByte || lowByte > MaxIpLowByte) {
    return "IP low byte must be from " + std::to_string(MinIpLowByte) + " to " +
           std::to_string(MaxIpLowByte);
  }
  return "";
}

std::string Validator::isCorrectIpStr(const std::string &ipStr) {
  auto parseResult = utils::parseIp(ipStr);
  if (parseResult) {
    return "";
  }
  return parseResult.error();
}

std::string Validator::isCorrectPort(int port) {
  if (port < MinAvailablePort || port > MaxAvaliablePort) {
    return "Port must be from " + std::to_string(MinAvailablePort) + " to " +
           std::to_string(MaxAvaliablePort);
  }
  return "";
}

std::string Validator::isCorrectPortStr(const std::string &portStr) {
  auto parseResult = utils::parsePort(portStr);
  if (parseResult) {
    return "";
  }
  return parseResult.error();
}

std::string Validator::isCorrectImei(const imei_t &imei) {
  return isCorrectDigitStr(imei, MinImeiLength, MaxImeiLength, "IMEI");
}

std::string Validator::isCorrectImsi(const imsi_t &imsi) {
  return isCorrectDigitStr(imsi, MimImsiLength, MaxImsiLength, "IMSI");
}

std::string Validator::isCorrectMsisdn(const msisdn_t &msisdn) {
  bool correct = true;

  if (msisdn.length() != MsisdnLength) {
    correct = false;
  } else {
    for (int i = 0; i < msisdn.length(); ++i) {
      char formatCh = MsisdnFormatStr[i];
      if (std::isdigit(formatCh) && msisdn[i] != formatCh ||
          formatCh == MsisdnAnyDigit && !std::isdigit(msisdn[i])) {
        correct = false;
        break;
      }
    }
  }

  return correct ? "" : "MSISDN must have format: " + MsisdnFormatStr;
}

std::string
Validator::isCorrectSpeedDialNumStr(const std::string &speedDialNumStr) {
  if (speedDialNumStr.size() != 1) {
    return "Speed dial num must contain only 1 digit";
  }
  return "";
}

std::string Validator::isCorrectSignal(unsigned int signal) {
  if (signal > common::constants::MaxSignalLevel) {
    return "Signal cant be bigger than " +
           std::to_string(common::constants::MaxSignalLevel);
  }
  return "";
}

std::string Validator::isPositiveNumber(double number,
                                        const std::string &name) {
  return (number > 0) ? "" : name + " must be a positive number";
}
} // namespace common
