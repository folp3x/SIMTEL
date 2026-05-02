#pragma once

#include <expected>
#include <nlohmann/json.hpp>

#include "common/constants.h"
#include "config.h"

class ConfigParser {
private:
  std::expected<std::string, std::string>
  parseIP(const nlohmann::json &json) const;
  std::expected<int, std::string> parsePort(const nlohmann::json &json) const;
  std::expected<std::string, std::string>
  parseIMEI(const nlohmann::json &json) const;
  std::expected<std::string, std::string>
  parseIMSI(const nlohmann::json &json) const;
  std::expected<std::array<double, Constants::LOCATION_COORDS_COUNT>,
                std::string>
  parseLoc(const nlohmann::json &json) const;

public:
  std::expected<Config, std::string> parse(const std::string &filePath) const;
};
