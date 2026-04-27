#include "config_parser.h"

#include <fstream>

#include "app/validator/validator.h"

bool ConfigParser::parseIP(const nlohmann::json &j, std::string &msg,
                           std::string &ip) {
  if (!j.contains("ip")) {
    msg = "'ip' required";
    return false;
  }

  if (!j["ip"].is_string()) {
    msg = "Error: 'ip' must be a string";
    return false;
  }

  std::string value = j["ip"];
  std::string error = Validator::isCorrectIP(value);
  if (!error.empty()) {
    msg = error;
    return false;
  }

  ip = value;
  return true;
}

bool ConfigParser::parsePort(const nlohmann::json &j, std::string &msg,
                             int &port) {
  if (!j.contains("port")) {
    msg = "'port' required";
    return false;
  }

  if (!j["port"].is_number_integer()) {
    msg = "'port' must be an integer";
    return false;
  }

  int value = j["port"];
  std::string error = Validator::isCorrectPort(value);
  if (!error.empty()) {
    msg = error;
    return false;
  }

  port = value;
  return true;
}

bool ConfigParser::parseIMEI(const nlohmann::json &j, std::string &msg,
                             std::string &imei) {
  if (!j.contains("imei")) {
    msg = "'imei' required";
    return false;
  }

  if (!j["imei"].is_string()) {
    msg = "'imei' must be a string";
    return false;
  }

  std::string value = j["imei"];
  std::string error = Validator::isCorrectIMEI(value);
  if (!error.empty()) {
    msg = error;
    return false;
  }

  imei = value;
  return true;
}

bool ConfigParser::parseIMSI(const nlohmann::json &j, std::string &msg,
                             std::string &imsi) {
  if (!j.contains("imsi")) {
    msg = "'imsi required";
    return false;
  }

  if (!j["imsi"].is_string()) {
    msg = "'imsi' must be a string";
    return false;
  }

  std::string value = j["imsi"];
  std::string error = Validator::isCorrectIMSI(value);
  if (!error.empty()) {
    msg = error;
    return false;
  }

  imsi = value;
  return true;
}

bool ConfigParser::parseLoc(const nlohmann::json &j, std::string &msg,
                            std::array<double, 3> &loc) {
  if (!j.contains("loc")) {
    msg = "'loc' required";
    return true;
  }

  if (!j["loc"].is_array()) {
    msg = "'loc' must be an array";
    return false;
  }

  if (j["loc"].size() != 3) {
    msg = "'loc' must have exactly 3 elements";
    return false;
  }

  for (size_t i = 0; i < 3; ++i) {
    if (!j["loc"][i].is_number()) {
      msg = "'loc' elements must be numbers";
      return false;
    }
    loc[i] = j["loc"][i];
  }

  return true;
}

bool ConfigParser::parse(const std::string &filePath, std::string &msg,
                         Config &config) {
  try {
    std::ifstream file(filePath);
    if (!file.is_open()) {
      msg = "Cant open file '" + filePath + "'";
      return false;
    }

    nlohmann::json j;
    file >> j;

    if (!parseIP(j, msg, config.ip))
      return false;
    if (!parsePort(j, msg, config.port))
      return false;
    if (!parseIMEI(j, msg, config.imei))
      return false;
    if (!parseIMSI(j, msg, config.imsi))
      return false;
    if (!parseLoc(j, msg, config.loc))
      return false;

    return true;

  } catch (const std::exception &e) {
    msg = "Invalid json";
    return false;
  }
}
