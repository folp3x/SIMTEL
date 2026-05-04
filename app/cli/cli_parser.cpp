#include "cli_parser.h"

#include "app/validator/validator.h"

// задает аргументы командной строки и правила их проверки
void CLIParser::initOptions() {
  ipOpt = cliApp.add_option("-a, --ip", config.ip, "Set IP address");
  ipOpt->check(Validator::isCorrectIpStr);
  ipOpt->type_name("IPv4");
  configOpts.push_back(ipOpt);

  portOpt = cliApp.add_option("-p, --port", config.port, "Set port");
  portOpt->check(Validator::isCorrectPortStr);
  portOpt->type_name("integer");
  configOpts.push_back(portOpt);

  imeiOpt = cliApp.add_option("-e, --imei", config.imei, "Set IMEI");
  imeiOpt->check(Validator::isCorrectIMEI);
  imeiOpt->type_name("char[15]");
  configOpts.push_back(imeiOpt);

  imsiOpt = cliApp.add_option("-i, --imsi", config.imsi, "Set IMSI");
  imsiOpt->check(Validator::isCorrectIMSI);
  imsiOpt->type_name("char[15]");
  configOpts.push_back(imsiOpt);

  locOpt = cliApp.add_option("-l, --loc", config.loc, "Set position vector");
  locOpt->type_name("x y z (real)");
  configOpts.push_back(locOpt);

  configFileOpt = cliApp.add_option("-k, --config", configFilePath,
                                    "Load config from specified JSON file");
  configFileOpt->check(Validator::isCorrectConfigPath);
  configFileOpt->type_name("string");

  nodesFileOpt = cliApp.add_option("-n, --nodes", nodesFilePath,
                                   "Load nodes from specified JSON file");
  nodesFileOpt->check(Validator::isCorrectNodesPath);
  nodesFileOpt->type_name("string");
}

// проверяет указана ли опция
bool CLIParser::isOptSet(CLI::Option *opt) { return opt && opt->count() > 0; }

CLIParser::CLIParser() { initOptions(); }

// парсит аргументы, передает в msg сообщение об ошибке или help
bool CLIParser::parse(int argc, char *argv[], std::string &msg,
                      bool &helpCalled) {
  try {
    cliApp.parse(argc, argv);
    return true;
  } catch (const CLI::ParseError &e) {
    if (e.get_name() == "CallForHelp") {
      msg = cliApp.help();
      helpCalled = true;
    } else {
      msg = e.what();
    }
    return false;
  }
}

// проверяет указаны ли все опции для полей config
bool CLIParser::allConfigOptsSet() const {
  return std::all_of(configOpts.begin(), configOpts.end(), isOptSet);
}

std::optional<std::string> CLIParser::getParsedConfigFilePath() const {
  if (isOptSet(configFileOpt)) {
    return configFilePath;
  }
  return std::nullopt;
}

std::optional<std::string> CLIParser::getParsedNodesFilePath() const {
  if (isOptSet(nodesFileOpt)) {
    return nodesFilePath;
  }
  return std::nullopt;
}

Config CLIParser::redefineConfig(const Config &definedConfig) {
  Config redefinedConfig = definedConfig;

  if (isOptSet(ipOpt))
    redefinedConfig.setIP(config.ip);
  if (isOptSet(portOpt))
    redefinedConfig.setPort(config.port);
  if (isOptSet(imeiOpt))
    redefinedConfig.setImei(config.imei);
  if (isOptSet(imsiOpt))
    redefinedConfig.setImsi(config.imsi);
  if (isOptSet(locOpt))
    redefinedConfig.setLoc(config.loc);

  return redefinedConfig;
}
