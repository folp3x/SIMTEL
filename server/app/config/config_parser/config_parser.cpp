#include "config_parser.h"

#include "common/json/info/json_field_info/json_field_info.h"
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
      [](unsigned int ttl) { return (ttl > 0) ? "" : "Ttl cant be 0"; }));
  smscConfigObj->addInner(std::move(cdrAccessParamsObj));

  addParsedObject(std::move(smscConfigObj));
}

void ConfigParser::initFields() {
  common::ConfigParser<Config>::initFields();
  initBsFilePathField();
  initEpcFilePathField();
  initSmscConfigField();
}

std::unique_ptr<ConfigParser> ConfigParser::create() {
  auto parser = std::unique_ptr<ConfigParser>(new ConfigParser());
  parser->initFields();
  return parser;
}
} // namespace server
