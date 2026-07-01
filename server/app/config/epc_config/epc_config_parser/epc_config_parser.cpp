#include "epc_config_parser.h"

#include "common/validator/validator.h"

namespace server {
void EpcConfigParser::initFields() {
  initTtlField();
  initHlrAccessParamsField();
  initEirAccessParamsField();
}

std::expected<EpcConfig, std::string>
EpcConfigParser::parseJson(const nlohmann::json &json) {
  config = {};

  auto error = this->parseFields(json);
  if (error) {
    return std::unexpected(*error);
  }

  return config;
}

void EpcConfigParser::initTtlField() {
  addParsedField<unsigned int>(
      "ttl_sec", [this](unsigned int ttl) { config.ttlSec = ttl; },
      [](unsigned int ttl) { return (ttl > 0) ? "" : "TTL cant be 0"; });
}

void EpcConfigParser::initHlrAccessParamsField() {
  auto hlrAccessParamsObj =
      std::make_unique<common::JsonObjectInfo>("hlrAccessParams");
  hlrAccessParamsObj->addInner(
      std::make_unique<common::JsonFieldInfo<std::string>>(
          "sqliteFilePath", [this](const std::string &path) {
            config.hlrSqliteFilePath = path;
          }));

  addParsedObject(std::move(hlrAccessParamsObj));
}

void EpcConfigParser::initEirAccessParamsField() {
  auto eirAccessParamsObj =
      std::make_unique<common::JsonObjectInfo>("eirAccessParams");
  eirAccessParamsObj->addInner(
      std::make_unique<common::JsonFieldInfo<std::string>>(
          "sqliteFilePath", [this](const std::string &path) {
            config.eirSqliteFilePath = path;
          }));

  addParsedObject(std::move(eirAccessParamsObj));
}

std::unique_ptr<EpcConfigParser> EpcConfigParser::create() {
  auto parser = std::unique_ptr<EpcConfigParser>(new EpcConfigParser());
  parser->initFields();
  return parser;
}
} // namespace server
