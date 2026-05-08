#include "cli_parser.h"

#include "common/validator/validator.h"

namespace client {
CLIParser::CLIParser(const std::string &cliAppName)
    : common::CLIParser<Config>(cliAppName) {}

void CLIParser::initImeiOpt() {
  imeiOpt = cliApp.add_option("-e, --imei", config.imei, "Set IMEI");
  imeiOpt->check(common::Validator::isCorrectIMEI);
  imeiOpt->type_name("char[15]");
  configOpts.push_back(imeiOpt);
}

void CLIParser::initImsiOpt() {
  imsiOpt = cliApp.add_option("-i, --imsi", config.imsi, "Set IMSI");
  imsiOpt->check(common::Validator::isCorrectIMSI);
  imsiOpt->type_name("char[15]");
  configOpts.push_back(imsiOpt);
}

void CLIParser::initNodesFileOpt() {
  nodesFileOpt = cliApp.add_option("-n, --nodes", nodesFilePath,
                                   "Load nodes from specified JSON file");
  nodesFileOpt->check(common::Validator::isCorrectNodesPath);
  nodesFileOpt->type_name("string");
}

void CLIParser::initOptions() {
  common::CLIParser<Config>::initOptions();
  initImeiOpt();
  initImsiOpt();
  initNodesFileOpt();
}

std::optional<std::string> CLIParser::getParsedNodesFilePath() const {
  if (isOptSet(nodesFileOpt)) {
    return nodesFilePath;
  }
  return std::nullopt;
}

std::unique_ptr<CLIParser> CLIParser::create() {
  auto parser = std::unique_ptr<CLIParser>(new CLIParser("Client"));
  parser->initOptions();
  return parser;
}

Config CLIParser::redefineConfig(const Config &definedConfig) const {
  Config redefinedConfig =
      common::CLIParser<Config>::redefineConfig(definedConfig);

  if (isOptSet(imeiOpt))
    redefinedConfig.setImei(config.imei);
  if (isOptSet(imsiOpt))
    redefinedConfig.setImsi(config.imsi);

  return redefinedConfig;
}
} // namespace client
