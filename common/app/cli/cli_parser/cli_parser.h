#pragma once

#include <CLI/CLI.hpp>
#include <algorithm>
#include <concepts>
#include <optional>
#include <vector>

#include "common/app/config/config/config.h"
#include "common/constants/constants.h"
#include "common/validator/validator.h"

namespace common {
// базовый класс для парсинга аргументов командной строки
template <std::derived_from<Config> T> class CLIParser {
private:
  std::string configFilePath = "";

protected:
  CLI::App cliApp{};

  T config{};

  // опции для получения параметров конфигурации
  CLI::Option *ipOpt = nullptr;
  CLI::Option *portOpt = nullptr;
  CLI::Option *locOpt = nullptr;

  std::vector<CLI::Option *> configOpts = {};

  CLI::Option *configFileOpt = nullptr;

  explicit CLIParser(const std::string &appTitle) : cliApp(appTitle) {}

  void initIpOption() {
    ipOpt = cliApp.add_option_function<std::string>(
        "-a, --ip", [this](const std::string &ip) { config.setIP(ip); },
        "Set IP address");
    ipOpt->check(Validator::isCorrectIpStr);
    ipOpt->type_name("IPv4");
    configOpts.push_back(ipOpt);
  }

  void initPortOption() {
    portOpt = cliApp.add_option_function<int>(
        "-p, --port", [this](int port) { config.setPort(port); }, "Set port");
    portOpt->check(Validator::isCorrectPortStr);
    portOpt->type_name("integer");
    configOpts.push_back(portOpt);
  }

  void initLocOption() {
    locOpt = cliApp.add_option_function<common::coords_t<float>>(
        "-l, --loc",
        [this](const common::coords_t<float> &loc) { config.setLoc(loc); },
        "Set position vector");
    locOpt->type_name("x y z (real)");
    configOpts.push_back(locOpt);
  }

  void initConfigFileOption() {
    configFileOpt = cliApp.add_option_function<std::string>(
        "-k, --config",
        [this](const std::string &filePath) { configFilePath = filePath; },
        "Load config from specified JSON file");
    configFileOpt->check(Validator::isCorrectConfigPath);
    configFileOpt->type_name("string");
  }

  virtual void initOptions() {
    initIpOption();
    initPortOption();
    initLocOption();
    initConfigFileOption();
  }

  static bool isOptSet(CLI::Option *opt) { return opt && opt->count() > 0; }

public:
  virtual ~CLIParser() = default;

  static std::unique_ptr<CLIParser> create(const std::string &cliAppName) {
    auto parser = std::make_unique<CLIParser>(cliAppName);
    parser->initOptions();
    return parser;
  }

  bool parse(int argc, char *argv[], std::string &msg, bool &helpCalled) {
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

  bool allConfigOptsSet() const {
    return std::all_of(configOpts.begin(), configOpts.end(), isOptSet);
  }

  std::optional<std::string> getParsedConfigFilePath() const {
    if (isOptSet(configFileOpt)) {
      return configFilePath;
    }
    return std::nullopt;
  }

  virtual T redefineConfig(const T &definedConfig) const {
    T redefinedConfig = definedConfig;

    if (isOptSet(ipOpt))
      redefinedConfig.setIP(config.getIP());
    if (isOptSet(portOpt))
      redefinedConfig.setPort(config.getPort());
    if (isOptSet(locOpt))
      redefinedConfig.setLoc(config.getLoc());

    return redefinedConfig;
  }
};
} // namespace common
