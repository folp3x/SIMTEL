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

  auto error = parseFields(json);
  if (error) {
    return std::unexpected(*error);
  }

  return config;
}

void EpcConfigParser::initTtlField() {
  addInfo(makeParsedField<unsigned int>(
      "ttl_sec", [this](unsigned int ttl) { config.ttlSec = ttl; },
      [](unsigned int ttl) {
        return common::Validator::isPositiveNumber(ttl, "TTL");
      }));
}

void EpcConfigParser::initHlrAccessParamsField() {
  auto hlrAccessParamsObj = makeParsedObject("hlrAccessParams");
  hlrAccessParamsObj->addInner(makeParsedField<std::string>(
      "sqliteFilePath",
      [this](const std::string &path) { config.hlrSqliteFilePath = path; }));

  addInfo(std::move(hlrAccessParamsObj));
}

void EpcConfigParser::initEirAccessParamsField() {
  auto eirAccessParamsObj = makeParsedObject("eirAccessParams");
  eirAccessParamsObj->addInner(makeParsedField<std::string>(
      "sqliteFilePath",
      [this](const std::string &path) { config.eirSqliteFilePath = path; }));

  addInfo(std::move(eirAccessParamsObj));
}

std::unique_ptr<EpcConfigParser> EpcConfigParser::create() {
  auto parser = std::unique_ptr<EpcConfigParser>(new EpcConfigParser());
  parser->initFields();
  return parser;
}
} // namespace server
