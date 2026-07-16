#include "cli_parser.h"

#include "client/app/locale/locale.h"
#include "common/validator/validator.h"

namespace client {
CliParser::CliParser(const std::string &cliAppName)
    : common::CliParser<Config>(cliAppName) {}

void CliParser::initLocOpt() {
  if constexpr (common::constants::LocationCoordsCount == 1) {
    locOpt = cliApp.add_option_function<float>(
        "-l, --loc", [this](float x) { config.setLoc({x}); },
        "Set position vector");
    locOpt->type_name("x (real)");
  } else {
    locOpt = cliApp.add_option_function<common::coords_t<>>(
        "-l, --loc",
        [this](const common::coords_t<> &loc) { config.setLoc(loc); },
        "Set position vector");
    locOpt->type_name("coords (real)");
  }
  configOpts.push_back(locOpt);
}

void CliParser::initOptions() {
  common::CliParser<Config>::initOptions();

  imeiOpt = cliApp.add_option_function<common::imei_t>(
      "-e, --imei",
      [this](const common::imei_t &imei) { config.setImei(imei); }, "Set IMEI");
  imeiOpt->check(common::Validator::isCorrectImei);
  imeiOpt->type_name("char[15]");
  configOpts.push_back(imeiOpt);

  imsiOpt = cliApp.add_option_function<common::imsi_t>(
      "-i, --imsi",
      [this](const common::imsi_t &imsi) { config.setImsi(imsi); }, "Set IMSI");
  imsiOpt->check(common::Validator::isCorrectImsi);
  imsiOpt->type_name("char[15]");
  configOpts.push_back(imsiOpt);

  ipOpt = cliApp.add_option_function<std::string>(
      "-a, --ip", [this](const std::string &ip) { config.setIp(ip); },
      "Set IP address");
  ipOpt->check(common::Validator::isCorrectIpStr);
  ipOpt->type_name("IPv4");
  configOpts.push_back(ipOpt);

  initLocOpt();

  localeOpt = cliApp.add_option("--lang", localeAlias, "Set language");
  localeOpt->check(isSupportedLocale);
  localeOpt->type_name("en, ru, etc.");
}

std::unique_ptr<CliParser> CliParser::create() {
  auto parser = std::unique_ptr<CliParser>(new CliParser("Client"));
  parser->initOptions();
  return parser;
}

Config CliParser::redefineConfig(const Config &definedConfig) const {
  Config redefinedConfig =
      common::CliParser<Config>::redefineConfig(definedConfig);

  if (isOptSet(imeiOpt))
    redefinedConfig.setImei(config.getImei());
  if (isOptSet(imsiOpt))
    redefinedConfig.setImsi(config.getImsi());
  if (isOptSet(ipOpt))
    redefinedConfig.setIp(config.getIp());
  if (isOptSet(locOpt))
    redefinedConfig.setLoc(config.getLoc());

  return redefinedConfig;
}

std::optional<std::string> CliParser::getLocaleAlias() const {
  bool set = isOptSet(localeOpt);
  return set ? std::optional{localeAlias} : std::nullopt;
}
} // namespace client
