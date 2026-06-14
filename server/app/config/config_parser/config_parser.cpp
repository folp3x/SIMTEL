#include "config_parser.h"

#include "common/json/info/json_field_info/json_field_info.h"
#include "common/json/info/json_object_array_info/json_object_array_info.h"
#include "common/json/info/json_object_info/json_object_info.h"
#include "common/validator/validator.h"

namespace server {
void ConfigParser::initBsFilePathField() {
  addParsedField<std::string>(
      "bsFilePath",
      [this](const std::string &path) { config.setBsFilePath(path); },
      [](const std::string &path) {
        return common::Validator::jsonFilePathExists(path, "BS");
      });
}

void ConfigParser::initEpcFilePathField() {
  addParsedField<std::string>(
      "epcFilePath",
      [this](const std::string &path) { config.setEpcFilePath(path); },
      [](const std::string &path) {
        return common::Validator::jsonFilePathExists(path, "EPC");
      });
}

void ConfigParser::initSmscConfigField() {
  auto cdrAccessParamsObj =
      std::make_unique<common::JsonObjectInfo>("cdrAccessParams");
  cdrAccessParamsObj->addInner(
      std::make_unique<common::JsonFieldInfo<std::string>>(
          "jsonFilePath", [this](const std::string &path) {
            config.setSmsCdrAccessJsonFilePath(path);
          }));

  auto smscConfigObj = std::make_unique<common::JsonObjectInfo>("smscConfig");
  smscConfigObj->addInner(std::make_unique<common::JsonFieldInfo<unsigned int>>(
      "smsTtl_ms", [this](unsigned int ttl) { config.setSmscTtlMs(ttl); },
      [](unsigned int ttl) { return (ttl > 0) ? "" : "SMS TTL cant be 0"; }));
  smscConfigObj->addInner(std::move(cdrAccessParamsObj));

  addParsedObject(std::move(smscConfigObj));
}

void ConfigParser::initMmeConfigsField() {
  common::JsonObjectInfo mmeConfigObj{""};
  mmeConfigObj.addInner(std::make_unique<common::JsonFieldInfo<unsigned int>>(
      "id", [this](unsigned int id) { curMmeConfig.id = id; }));
  mmeConfigObj.addInner(std::make_unique<common::JsonFieldInfo<size_t>>(
      "maxVlrSize", [this](size_t size) { curMmeConfig.maxVlrSize = size; },
      [](size_t size) { return (size > 0) ? "" : "VLR size cant be 0"; }));

  addParsedObjectArray("mmeConfigs", std::move(mmeConfigObj), [this]() {
    config.addMmeConfig(curMmeConfig);
    curMmeConfig = {};
  });
}

void ConfigParser::initFields() {
  common::ConfigParser<Config>::initFields();
  initBsFilePathField();
  initEpcFilePathField();
  initSmscConfigField();
  initMmeConfigsField();
}

std::unique_ptr<ConfigParser> ConfigParser::create() {
  auto parser = std::unique_ptr<ConfigParser>(new ConfigParser());
  parser->initFields();
  return parser;
}
} // namespace server
