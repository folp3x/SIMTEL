#include "config_parser.h"

#include "common/json/info/json_field_info/json_field_info.h"
#include "common/json/info/json_object_array_info/json_object_array_info.h"
#include "common/json/info/json_object_info/json_object_info.h"
#include "common/validator/validator.h"

namespace server {
void ConfigParser::initBsFilePathField() {
  addInfo(makeParsedField<std::string>(
      "bsFilePath",
      [this](const std::string &path) { config.setBsFilePath(path); },
      [](const std::string &path) {
        return common::Validator::jsonFilePathExists(path, "BS");
      }));
}

void ConfigParser::initEpcFilePathField() {
  addInfo(makeParsedField<std::string>(
      "epcFilePath",
      [this](const std::string &path) { config.setEpcFilePath(path); },
      [](const std::string &path) {
        return common::Validator::jsonFilePathExists(path, "EPC");
      }));
}

void ConfigParser::initSmscConfigField() {
  auto smscConfigObj = makeParsedObject("smscConfig");
  smscConfigObj->addInner(makeParsedField<unsigned int>(
      "smsTtl_ms", [this](unsigned int ttl) { config.setSmscTtlMs(ttl); },
      [](unsigned int ttl) {
        return common::Validator::isPositiveNumber(ttl, "SMS TTL");
      }));

  addInfo(std::move(smscConfigObj));
}

void ConfigParser::initMmeConfigsField() {
  auto mmeConfigObj = makeParsedObject("");
  mmeConfigObj->addInner(makeParsedField<unsigned int>(
      "id", [this](unsigned int id) { curMmeConfig.id = id; }));
  mmeConfigObj->addInner(makeParsedField<size_t>(
      "maxVlrSize", [this](size_t size) { curMmeConfig.maxVlrSize = size; },
      [](size_t size) {
        return common::Validator::isPositiveNumber(size, "VLR size");
      }));

  addInfo(
      makeParsedObjectArray("mmeConfigs", std::move(mmeConfigObj), [this]() {
        config.addMmeConfig(curMmeConfig);
        curMmeConfig = {};
      }));
}

void ConfigParser::initPcrfConfigField() {
  auto pcrfConfigObj = makeParsedObject("pcrfConfig");
  pcrfConfigObj->addInner(makeParsedField<double>(
      "smsPrice_rub",
      [this](double price) { config.setPcrfSmsPriceRub(price); },
      [](double price) {
        return common::Validator::isPositiveNumber(price, "SMS price");
      }));

  auto balanceInfoObj = makeParsedObject("");
  balanceInfoObj->addInner(makeParsedField<common::imsi_t>(
      "imsi",
      [this](const common::imsi_t &imsi) { curBalanceInfo.imsi = imsi; },
      common::Validator::isCorrectImsi));
  balanceInfoObj->addInner(
      makeParsedField<double>("balance_rub", [this](double balance) {
        curBalanceInfo.balanceRub = balance;
      }));

  pcrfConfigObj->addInner(
      makeParsedObjectArray("balanceInfo", std::move(balanceInfoObj), [this]() {
        config.addPcrfBalanceInfo(curBalanceInfo);
        curBalanceInfo = {};
      }));

  addInfo(std::move(pcrfConfigObj));
}

void ConfigParser::initFields() {
  common::ConfigParser<Config>::initFields();
  initBsFilePathField();
  initEpcFilePathField();
  initSmscConfigField();
  initMmeConfigsField();
  initPcrfConfigField();
}

std::unique_ptr<ConfigParser> ConfigParser::create() {
  auto parser = std::unique_ptr<ConfigParser>(new ConfigParser());
  parser->initFields();
  return parser;
}
} // namespace server
