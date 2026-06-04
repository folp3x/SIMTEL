#pragma once

#include <algorithm>

#include "common/validator/validator.h"

namespace common {
template <std::derived_from<Config> T>
CLIParser<T>::CLIParser(const std::string &appTitle) : cliApp(appTitle) {}

template <std::derived_from<Config> T> void CLIParser<T>::initIpOption() {
  ipOpt = cliApp.add_option_function<std::string>(
      "-a, --ip", [this](const std::string &ip) { config.setIP(ip); },
      "Set IP address");
  ipOpt->check(Validator::isCorrectIpStr);
  ipOpt->type_name("IPv4");
  configOpts.push_back(ipOpt);
}

template <std::derived_from<Config> T> void CLIParser<T>::initPortOption() {
  portOpt = cliApp.add_option_function<int>(
      "-p, --port", [this](int port) { config.setPort(port); }, "Set port");
  portOpt->check(Validator::isCorrectPortStr);
  portOpt->type_name("integer");
  configOpts.push_back(portOpt);
}

template <std::derived_from<Config> T> void CLIParser<T>::initLocOption() {
  if constexpr (common::constants::LOCATION_COORDS_COUNT == 1) {
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

template <std::derived_from<Config> T>
void CLIParser<T>::initConfigFileOption() {
  configFileOpt = cliApp.add_option_function<std::string>(
      "-k, --config",
      [this](const std::string &filePath) { configFilePath = filePath; },
      "Load config from specified JSON file");
  configFileOpt->check(Validator::isCorrectConfigPath);
  configFileOpt->type_name("string");
}

template <std::derived_from<Config> T> void CLIParser<T>::initOptions() {
  initIpOption();
  initPortOption();
  initLocOption();
  initConfigFileOption();
}

template <std::derived_from<Config> T>
bool CLIParser<T>::isOptSet(CLI::Option *opt) {
  return opt && opt->count() > 0;
}

template <std::derived_from<Config> T>
std::unique_ptr<CLIParser<T>>
CLIParser<T>::create(const std::string &cliAppName) {
  auto parser = std::make_unique<CLIParser<T>>(cliAppName);
  parser->initOptions();
  return parser;
}

template <std::derived_from<Config> T>
bool CLIParser<T>::parse(int argc, char *argv[], std::string &msg,
                         bool &helpCalled) {
  try {
    // очистка конфига
    config = T{};
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

template <std::derived_from<Config> T> bool CLIParser<T>::allConfigOptsSet() {
  return std::all_of(configOpts.begin(), configOpts.end(),
                     [](CLI::Option *opt) { return isOptSet(opt); });
}

template <std::derived_from<Config> T>
std::optional<std::string> CLIParser<T>::getParsedConfigFilePath() const {
  if (isOptSet(configFileOpt)) {
    return configFilePath;
  }
  return std::nullopt;
}

template <std::derived_from<Config> T>
T CLIParser<T>::redefineConfig(const T &definedConfig) const {
  T redefinedConfig = definedConfig;

  if (isOptSet(ipOpt))
    redefinedConfig.setIP(config.getIP());
  if (isOptSet(portOpt))
    redefinedConfig.setPort(config.getPort());
  if (isOptSet(locOpt))
    redefinedConfig.setLoc(config.getLoc());

  return redefinedConfig;
}
} // namespace common
