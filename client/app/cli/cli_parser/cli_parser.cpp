#include "cli_parser.h"

#include "common/validator/validator.h"

namespace client {
CLIParser::CLIParser(const std::string &cliAppName)
    : common::CLIParser<Config>(cliAppName) {}

void CLIParser::initImeiOpt() {
  imeiOpt = cliApp.add_option_function<common::imei_t>(
      "-e, --imei",
      [this](const common::imei_t &imei) { config.setImei(imei); }, "Set IMEI");
  imeiOpt->check(common::Validator::isCorrectIMEI);
  imeiOpt->type_name("char[15]");
  configOpts.push_back(imeiOpt);
}

void CLIParser::initImsiOpt() {
  imsiOpt = cliApp.add_option_function<common::imsi_t>(
      "-i, --imsi",
      [this](const common::imsi_t &imsi) { config.setImsi(imsi); }, "Set IMSI");
  imsiOpt->check(common::Validator::isCorrectIMSI);
  imsiOpt->type_name("char[15]");
  configOpts.push_back(imsiOpt);
}

void CLIParser::initIpOpt() {
  ipOpt = cliApp.add_option_function<std::string>(
      "-a, --ip", [this](const std::string &ip) { config.setIP(ip); },
      "Set IP address");
  ipOpt->check(common::Validator::isCorrectIpStr);
  ipOpt->type_name("IPv4");
  configOpts.push_back(ipOpt);
}

void CLIParser::initOptions() {
  common::CLIParser<Config>::initOptions();
  initImeiOpt();
  initImsiOpt();
  initIpOpt();
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
    redefinedConfig.setImei(config.getImei());
  if (isOptSet(imsiOpt))
    redefinedConfig.setImsi(config.getImsi());
  if (isOptSet(ipOpt))
    redefinedConfig.setIP(config.getIP());

  return redefinedConfig;
}
} // namespace client
