#pragma once

#include <CLI/CLI.hpp>
#include <concepts>
#include <optional>
#include <vector>

#include "common/app/config/config/config.h"

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

  explicit CLIParser(const std::string &appTitle);

  void initIpOption();
  void initPortOption();
  void initLocOption();
  void initConfigFileOption();

  virtual void initOptions();

  static bool isOptSet(CLI::Option *opt);

public:
  virtual ~CLIParser() = default;

  static std::unique_ptr<CLIParser> create(const std::string &cliAppName);

  bool parse(int argc, char *argv[], std::string &msg, bool &helpCalled);

  bool allConfigOptsSet();

  std::optional<std::string> getParsedConfigFilePath() const;

  virtual T redefineConfig(const T &definedConfig) const;
};
} // namespace common

#include "cli_parser_impl.h"
