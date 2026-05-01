#include "validator.h"

#include <arpa/inet.h>
#include <cstring>
#include <filesystem>

#include "utils/str.h"

// проверяет что путь является путем к файлу JSON
bool Validator::isCorrectJsonPath(const std::string &filePath) {
  int jsonExtLen = std::strlen(".json");
  return (filePath.size() < jsonExtLen) ||
         filePath.substr(filePath.size() - jsonExtLen) == ".json";
}

std::string Validator::isCorrectIP(const std::string &ip) {
  sockaddr_in stubSa;
  if (inet_pton(AF_INET, ip.c_str(), &(stubSa.sin_addr)) != 1) {
    return "Incorrect IP address";
  }
  return "";
}

std::string Validator::isCorrectPort(int port) {
  if (port < MIN_AVAILABLE_PORT && port > MAX_AVAILABLE_PORT) {
    return "Port must be from " + std::to_string(MIN_AVAILABLE_PORT) + " to " +
           std::to_string(MAX_AVAILABLE_PORT);
  }
  return "";
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
