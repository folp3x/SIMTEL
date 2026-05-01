#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <nlohmann/json.hpp>

#include "common/constants.h"
#include "config.h"

class ConfigParser {
private:
  bool parseIP(const nlohmann::json &json, std::string &msg, std::string &ip);
  bool parsePort(const nlohmann::json &json, std::string &msg, int &port);
  bool parseIMEI(const nlohmann::json &json, std::string &msg,
                 std::string &imei);
  bool parseIMSI(const nlohmann::json &json, std::string &msg,
                 std::string &imsi);
  bool parseLoc(const nlohmann::json &json, std::string &msg,
                std::array<double, Constants::LOCATION_COORDS_COUNT> &loc);

public:
  bool parse(const std::string &filePath, std::string &msg, Config &config);
};

#endif // CONFIG_PARSER_H
