#include "config_parser.h"

#include "client/validator/validator.h"
#include "common/validator/validator.h"

namespace client {
void ConfigParser::initImeiField() {
  addInfo(makeParsedField<common::imei_t>(
      "imei", [this](const common::imei_t &imei) { config.setImei(imei); },
      common::Validator::isCorrectImei));
}

void ConfigParser::initImsiField() {
  addInfo(makeParsedField<common::imsi_t>(
      "imsi", [this](const common::imei_t &imsi) { config.setImsi(imsi); },
      common::Validator::isCorrectImsi));
}

void ConfigParser::initIpField() {
  addInfo(makeParsedField<std::string>(
      "ip", [this](const std::string &ip) { config.setIp(ip); },
      common::Validator::isCorrectIpStr));
}

void ConfigParser::initAddressBookFilePathField() {
  addInfo(makeParsedField<std::string>(
      "addressBookFilePath",
      [this](const std::string &filePath) {
        config.setAddressBookFilePath(filePath);
      },
      [](const std::string &filePath) {
        return common::Validator::jsonFilePathExists(filePath, "Address book");
      }));
}

void ConfigParser::initLocField() {
  addInfo(makeParsedArray<float, common::constants::LOCATION_COORDS_COUNT>(
      "loc", [this](const common::coords_t<> &loc) { config.setLoc(loc); }));
}

void ConfigParser::initFields() {
  common::ConfigParser<Config>::initFields();
  initImeiField();
  initImsiField();
  initIpField();
  initAddressBookFilePathField();
  initLocField();
}

std::unique_ptr<ConfigParser> ConfigParser::create() {
  auto parser = std::unique_ptr<ConfigParser>(new ConfigParser());
  parser->initFields();
  return parser;
}
} // namespace client
