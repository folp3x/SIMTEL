#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>

class Validator {
private:
  static const int MIN_AVAILABLE_PORT = 1024;
  static const int MAX_AVAILABLE_PORT = 65535;
  static const int IMEI_LENGTH = 15;
  static const int IMSI_LENGTH = 15;

  static bool allDigits(const std::string &str);
  static bool isCorrectJsonPath(const std::string &filePath);

public:
  static std::string isCorrectIP(const std::string &ip);
  static std::string isCorrectPort(int port);
  static std::string isCorrectIMEI(const std::string &imei);
  static std::string isCorrectIMSI(const std::string &imsi);
  static std::string isCorrectConfigPath(const std::string &filePath);
  static std::string isCorrectNodesPath(const std::string &filePath);
};

#endif // VALIDATOR_H
