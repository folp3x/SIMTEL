#include "config_parser.h"

#include "client/validator/validator.h"
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

void ConfigParser::initIpField() {
  this->template addParsedField<std::string>(
      "ip", [this](const std::string &ip) { config.setIP(ip); },
      nlohmann::json::value_t::string, common::Validator::isCorrectIpStr);
}

void ConfigParser::initAddressBookFilePathField() {
  this->template addParsedField<std::string>(
      "addressBookFilePath",
      [this](const std::string &filePath) {
        config.setAddressBookFilePath(filePath);
      },
      nlohmann::json::value_t::string,
      client::Validator::isCorrectAddressBookFilePath);
}

void ConfigParser::initFields() {
  common::ConfigParser<Config>::initFields();
  initImeiField();
  initImsiField();
  initIpField();
  initAddressBookFilePathField();
}

std::unique_ptr<ConfigParser> ConfigParser::create() {
  auto parser = std::unique_ptr<ConfigParser>(new ConfigParser());
  parser->initFields();
  return parser;
}
} // namespace client
