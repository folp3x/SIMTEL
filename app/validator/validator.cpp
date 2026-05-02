#include "validator.h"

#include <cstring>
#include <filesystem>

#include "utils/network.h"
#include "utils/str.h"

// проверяет что путь является путем к файлу JSON
bool Validator::isCorrectJsonPath(const std::string &filePath) {
  int jsonExtLen = std::strlen(".json");
  return (filePath.size() < jsonExtLen) ||
         filePath.substr(filePath.size() - jsonExtLen) == ".json";
}

std::string Validator::isCorrectIP(uint32_t ip) {
  uint8_t lowByte = ip | 0xFF;
  if (lowByte < MIN_IP_LOW_BYTE || lowByte > MIN_IP_LOW_BYTE) {
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

std::string Validator::isCorrectIMEI(const std::string &imei) {
  if (imei.size() != IMEI_LENGTH) {
    return "IMEI must have " + std::to_string(IMEI_LENGTH) + " digits ";
  } else if (!allDigits(imei)) {
    return "IMEI must contain only digits";
  }

  return "";
}

std::string Validator::isCorrectIMSI(const std::string &imsi) {
  if (imsi.size() != IMSI_LENGTH) {
    return "IMSI must have " + std::to_string(IMSI_LENGTH) + " digits ";
  } else if (!allDigits(imsi)) {
    return "IMSI must contain only digits";
  }

  return "";
}

std::string Validator::isCorrectConfigPath(const std::string &filePath) {
  if (!isCorrectJsonPath(filePath)) {
    return "Config path must be a path to .json file";
  } else if (!std::filesystem::exists(filePath)) {
    return "Config file not found";
  }
  return "";
}

std::string Validator::isCorrectNodesPath(const std::string &filePath) {
  if (!isCorrectJsonPath(filePath)) {
    return "Nodes path must be a path to .json file";
  } else if (!std::filesystem::exists(filePath)) {
    return "Nodes file not found";
  }
  return "";
}
