#include "config_parser.h"

#include <fstream>

#include "common/validator/validator.h"

namespace client {
void ConfigParser::initImeiField() {
  addParsedField<std::string>("imei", &config.imei,
                              nlohmann::detail::value_t::string,
                              common::Validator::isCorrectIMEI);
}

void ConfigParser::initImsiField() {
  addParsedField<std::string>("imsi", &config.imsi,
                              nlohmann::detail::value_t::string,
                              common::Validator::isCorrectIMSI);
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
