#pragma once

#include "common/app/cli_parser/cli_parser.h"

#include "client/app/config/config/config.h"

namespace client {
class CliParser : public common::CliParser<Config> {
private:
  explicit CliParser(const std::string &cliAppName);

  CLI::Option *imeiOpt = nullptr;
  CLI::Option *imsiOpt = nullptr;
  CLI::Option *ipOpt = nullptr;
  CLI::Option *locOpt = nullptr;
  CLI::Option *localeOpt = nullptr;

  std::string localeAlias = "";

  void initLocOpt();

  virtual void initOptions() override;

public:
  static std::unique_ptr<CliParser> create();

  virtual Config redefineConfig(const Config &definedConfig) const override;

  std::optional<std::string> getLocaleAlias() const;
};
} // namespace client
