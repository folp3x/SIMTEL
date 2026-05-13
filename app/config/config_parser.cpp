#include "config_parser.h"

#include <fstream>

#include "app/validator/validator.h"

std::expected<std::string, std::string>
ConfigParser::parseIP(const nlohmann::json &json) const {
  if (!json.contains("ip"))
    return std::unexpected("'ip' required");

  if (!json["ip"].is_string())
    return std::unexpected("'ip' must be a string");

  std::string ip = json["ip"];

  std::string error = Validator::isCorrectIpStr(ip);
  if (!error.empty())
    return std::unexpected(error);

  return ip;
}

std::expected<int, std::string>
ConfigParser::parsePort(const nlohmann::json &json) const {
  if (!json.contains("port"))
    return std::unexpected("'port' required");

  if (!json["port"].is_number_integer())
    return std::unexpected("'port' must be an integer");

  int port = json["port"];

  std::string error = Validator::isCorrectPort(port);
  if (!error.empty())
    return std::unexpected(error);

  return port;
}

std::expected<std::string, std::string>
ConfigParser::parseIMEI(const nlohmann::json &json) const {
  if (!json.contains("imei"))
    return std::unexpected("'imei' required");

  if (!json["imei"].is_string())
    return std::unexpected("'imei' must be a string");

  std::string imei = json["imei"];

  std::string error = Validator::isCorrectIMEI(imei);
  if (!error.empty())
    return std::unexpected(error);

  return imei;
}

std::expected<std::string, std::string>
ConfigParser::parseIMSI(const nlohmann::json &json) const {
  if (!json.contains("imsi"))
    return std::unexpected("'imsi required");

  if (!json["imsi"].is_string())
    return std::unexpected("'imsi' must be a string");

  std::string imsi = json["imsi"];

  std::string error = Validator::isCorrectIMSI(imsi);
  if (!error.empty())
    return std::unexpected(error);

  return imsi;
}

std::expected<std::array<double, Constants::LOCATION_COORDS_COUNT>, std::string>
ConfigParser::parseLoc(const nlohmann::json &json) const {
  if (!json.contains("loc"))
    return std::unexpected("'loc' required");

  if (!json["loc"].is_array())
    return std::unexpected("'loc' must be an array");

  if (json["loc"].size() != Constants::LOCATION_COORDS_COUNT)
    return std::unexpected("'loc' must have exactly " +
                           std::to_string(Constants::LOCATION_COORDS_COUNT) +
                           " elements");

  std::array<double, Constants::LOCATION_COORDS_COUNT> loc = {};
  for (size_t i = 0; i < Constants::LOCATION_COORDS_COUNT; ++i) {
    if (!json["loc"][i].is_number())
      return std::unexpected("'loc' elements must be numbers");

    loc[i] = json["loc"][i];
  }

  return loc;
}

std::expected<Config, std::string>
ConfigParser::parse(const std::string &filePath) const {
  std::ifstream file(filePath);
  if (!file.is_open())
    return std::unexpected("Cant open file '" + filePath + "'");

  nlohmann::json json;

  try {
    file >> json;

    if (file.fail())
      return std::unexpected("File error");

    Config config{};

    // ip
    if (auto ip = parseIP(json))
      config.setIP(*ip);
    else
      return std::unexpected(ip.error());

    // port
    if (auto port = parsePort(json))
      config.setPort(*port);
    else
      return std::unexpected(port.error());

    // imei
    if (auto imei = parseIMEI(json))
      config.setImei(*imei);
    else
      return std::unexpected(imei.error());

    // imsi
    if (auto imsi = parseIMSI(json))
      config.setImsi(*imsi);
    else
      return std::unexpected(imsi.error());

    // loc
    if (auto loc = parseLoc(json))
      config.setLoc(*loc);
    else
      return std::unexpected(loc.error());

    return config;
  } catch (const nlohmann::json::parse_error &e) {
    return std::unexpected("JSON parse error: " + std::string(e.what()));
  } catch (const nlohmann::json::type_error &e) {
    return std::unexpected("JSON type error: " + std::string(e.what()));
  } catch (const nlohmann::json::out_of_range &e) {
    return std::unexpected("JSON out of range error: " + std::string(e.what()));
  } catch (const std::exception &e) {
    return std::unexpected("JSON error: " + std::string(e.what()));
  }
}
