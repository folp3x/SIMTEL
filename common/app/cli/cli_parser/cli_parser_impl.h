#pragma once

#include <algorithm>

#include "common/validator/validator.h"

namespace common {
template <std::derived_from<Config> T>
CliParser<T>::CliParser(const std::string &appTitle) : cliApp(appTitle) {}

template <std::derived_from<Config> T> void CliParser<T>::initPortOpt() {
  portOpt = cliApp.add_option_function<int>(
      "-p, --port", [this](int port) { config.setPort(port); }, "Set port");
  portOpt->check(Validator::isCorrectPortStr);
  portOpt->type_name("integer");
  configOpts.push_back(portOpt);
}

template <std::derived_from<Config> T> void CliParser<T>::initConfigFileOpt() {
  configFileOpt = cliApp.add_option_function<std::string>(
      "-k, --config",
      [this](const std::string &filePath) { configFilePath = filePath; },
      "Load config from specified JSON file");
  configFileOpt->check([](const std::string &filePath) {
    return Validator::jsonFilePathExists(filePath, "Config");
  });
  configFileOpt->type_name("string");
}

template <std::derived_from<Config> T> void CliParser<T>::initOptions() {
  initPortOpt();
  initConfigFileOpt();
}

template <std::derived_from<Config> T>
bool CliParser<T>::isOptSet(CLI::Option *opt) {
  return opt && opt->count() > 0;
}

template <std::derived_from<Config> T>
std::unique_ptr<CliParser<T>>
CliParser<T>::create(const std::string &cliAppName) {
  auto parser = std::unique_ptr<CliParser<T>>(new CliParser<T>(cliAppName));
  parser->initOptions();
  return parser;
}

template <std::derived_from<Config> T>
bool CliParser<T>::parse(int argc, char *argv[], std::string &msg,
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

template <std::derived_from<Config> T>
bool CliParser<T>::allConfigOptsSet() const {
  return std::all_of(configOpts.begin(), configOpts.end(),
                     [](CLI::Option *opt) { return isOptSet(opt); });
}

template <std::derived_from<Config> T>
std::optional<std::string> CliParser<T>::getConfigFilePath() const {
  if (isOptSet(configFileOpt)) {
    return configFilePath;
  }
  return std::nullopt;
}

template <std::derived_from<Config> T>
T CliParser<T>::redefineConfig(const T &definedConfig) const {
  T redefinedConfig = definedConfig;

  if (isOptSet(portOpt))
    redefinedConfig.setPort(config.getPort());

  return redefinedConfig;
}
} // namespace common
