#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <nlohmann/json.hpp>

#include "config.h"

class ConfigParser {
private:
  bool parseIP(const nlohmann::json &j, std::string &msg, std::string &ip);
  bool parsePort(const nlohmann::json &j, std::string &msg, int &port);
  bool parseIMEI(const nlohmann::json &j, std::string &msg, std::string &imei);
  bool parseIMSI(const nlohmann::json &j, std::string &msg, std::string &imsi);
  bool parseLoc(const nlohmann::json &j, std::string &msg,
                std::array<double, 3> &loc);

public:
  bool parse(const std::string &filePath, std::string &msg, Config &config);
};

#endif // CONFIG_PARSER_H
