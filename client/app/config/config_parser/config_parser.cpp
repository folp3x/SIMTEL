#include "config_parser.h"

#include <fstream>

#include "common/validator/validator.h"

namespace client {
void ConfigParser::initImeiField() {
  addParsedField<common::imei_t>(
      "imei", [this](const common::imei_t &imei) { config.setImei(imei); },
      nlohmann::json::value_t::string, common::Validator::isCorrectIMEI);
}

void ConfigParser::initImsiField() {
  addParsedField<common::imsi_t>(
      "imsi", [this](const common::imei_t &imsi) { config.setImsi(imsi); },
      nlohmann::json::value_t::string, common::Validator::isCorrectIMSI);
}

void ConfigParser::initFields() {
  common::ConfigParser<Config>::initFields();
  initImeiField();
  initImsiField();
}

std::unique_ptr<ConfigParser> ConfigParser::create() {
  auto parser = std::unique_ptr<ConfigParser>(new ConfigParser());
  parser->initFields();
  return parser;
}
} // namespace client
