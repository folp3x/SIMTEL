#include "config_parser.h"

#include <fstream>

#include "app/validator/validator.h"

bool ConfigParser::parseIP(const nlohmann::json &json, std::string &msg,
                           std::string &ip) {
  if (!json.contains("ip")) {
    msg = "'ip' required";
    return false;
  }

  if (!json["ip"].is_string()) {
    msg = "'ip' must be a string";
    return false;
  }

  std::string value = json["ip"];
  std::string error = Validator::isCorrectIP(value);
  if (!error.empty()) {
    msg = error;
    return false;
  }

  ip = value;
  return true;
}

bool ConfigParser::parsePort(const nlohmann::json &json, std::string &msg,
                             int &port) {
  if (!json.contains("port")) {
    msg = "'port' required";
    return false;
  }

  if (!json["port"].is_number_integer()) {
    msg = "'port' must be an integer";
    return false;
  }

  int value = json["port"];
  std::string error = Validator::isCorrectPort(value);
  if (!error.empty()) {
    msg = error;
    return false;
  }

  port = value;
  return true;
}

bool ConfigParser::parseIMEI(const nlohmann::json &json, std::string &msg,
                             std::string &imei) {
  if (!json.contains("imei")) {
    msg = "'imei' required";
    return false;
  }

  if (!json["imei"].is_string()) {
    msg = "'imei' must be a string";
    return false;
  }

  std::string value = json["imei"];
  std::string error = Validator::isCorrectIMEI(value);
  if (!error.empty()) {
    msg = error;
    return false;
  }

  imei = value;
  return true;
}

bool ConfigParser::parseIMSI(const nlohmann::json &json, std::string &msg,
                             std::string &imsi) {
  if (!json.contains("imsi")) {
    msg = "'imsi required";
    return false;
  }

  if (!json["imsi"].is_string()) {
    msg = "'imsi' must be a string";
    return false;
  }

  std::string value = json["imsi"];
  std::string error = Validator::isCorrectIMSI(value);
  if (!error.empty()) {
    msg = error;
    return false;
  }

  imsi = value;
  return true;
}

bool ConfigParser::parseLoc(
    const nlohmann::json &json, std::string &msg,
    std::array<double, Constants::LOCATION_COORDS_COUNT> &loc) {
  if (!json.contains("loc")) {
    msg = "'loc' required";
    return true;
  }

  if (!json["loc"].is_array()) {
    msg = "'loc' must be an array";
    return false;
  }

  if (json["loc"].size() != Constants::LOCATION_COORDS_COUNT) {
    msg = "'loc' must have exactly" +
          std::to_string(Constants::LOCATION_COORDS_COUNT) + "elements";
    return false;
  }

  for (size_t i = 0; i < Constants::LOCATION_COORDS_COUNT; ++i) {
    if (!json["loc"][i].is_number()) {
      msg = "'loc' elements must be numbers";
      return false;
    }
    loc[i] = json["loc"][i];
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

    nlohmann::json json;
    file >> json;

    if (!parseIP(json, msg, config.ip))
      return false;
    if (!parsePort(json, msg, config.port))
      return false;
    if (!parseIMEI(json, msg, config.imei))
      return false;
    if (!parseIMSI(json, msg, config.imsi))
      return false;
    if (!parseLoc(json, msg, config.loc))
      return false;

    return true;

  } catch (const std::exception &e) {
    msg = "Invalid json";
    return false;
  }
}
