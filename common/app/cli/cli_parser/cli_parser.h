#pragma once

#include <CLI/CLI.hpp>
#include <concepts>
#include <optional>
#include <vector>

#include "common/app/config/config/config.h"

namespace common {
template <std::derived_from<Config> T> class CliParser {
private:
  std::string configFilePath = "";

protected:
  CLI::App cliApp{};

  T config{};

  CLI::Option *portOpt = nullptr;

  std::vector<CLI::Option *> configOpts = {};

  CLI::Option *configFileOpt = nullptr;

  explicit CliParser(const std::string &appTitle);

  virtual void initOptions();

  static bool isOptSet(CLI::Option *opt);

  void initPortOpt();
  void initConfigFileOpt();

public:
  virtual ~CliParser() = default;

  static std::unique_ptr<CliParser> create(const std::string &cliAppName);

  virtual T redefineConfig(const T &definedConfig) const;

  bool parse(int argc, char *argv[], std::string &msg, bool &helpCalled);

  bool allConfigOptsSet() const;

  std::optional<std::string> getConfigFilePath() const;
};
} // namespace common

#include "cli_parser_impl.h"
