#pragma once

#include <CLI/CLI.hpp>
#include <algorithm>
#include <concepts>
#include <optional>
#include <vector>

#include "common/app/config/config.h"
#include "common/constants.h"
#include "common/validator/validator.h"

namespace common {
// базовый класс для парсинга аргументов командной строки
template <std::derived_from<Config> T> class CLIParser {
private:
  // путь к файлу конфигурации
  std::string configFilePath = "";

protected:
  CLI::App cliApp{};

  T config{};

  // опции для получения параметров конфигурации
  CLI::Option *ipOpt = nullptr;
  CLI::Option *portOpt = nullptr;
  CLI::Option *locOpt = nullptr;

  std::vector<CLI::Option *> configOpts = {};

  // опция для получения пути к файлу
  CLI::Option *configFileOpt = nullptr;

  explicit CLIParser(const std::string &appTitle) : cliApp(appTitle) {}

  virtual void initIpOption() {
    ipOpt = cliApp.add_option("-a, --ip", config.ip, "Set IP address");
    ipOpt->check(Validator::isCorrectIpStr);
    ipOpt->type_name("IPv4");
    configOpts.push_back(ipOpt);
  }

  virtual void initPortOption() {
    portOpt = cliApp.add_option("-p, --port", config.port, "Set port");
    portOpt->check(Validator::isCorrectPortStr);
    portOpt->type_name("integer");
    configOpts.push_back(portOpt);
  }

  virtual void initLocOption() {
    locOpt = cliApp.add_option("-l, --loc", config.loc, "Set position vector");
    locOpt->type_name("x y z (real)");
    configOpts.push_back(locOpt);
  }

  virtual void initConfigFileOption() {
    configFileOpt = cliApp.add_option("-k, --config", configFilePath,
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
      redefinedConfig.setIP(config.ip);
    if (isOptSet(portOpt))
      redefinedConfig.setPort(config.port);
    if (isOptSet(locOpt))
      redefinedConfig.setLoc(config.loc);

    return redefinedConfig;
  }
};
} // namespace common
