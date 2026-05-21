#pragma once

#include "common/app/cli/cli_parser/cli_parser.h"

#include "client/app/config/config/config.h"

namespace client {
class CLIParser : public common::CLIParser<Config> {
private:
  explicit CLIParser(const std::string &cliAppName);

  // опции для получения параметров конфига
  CLI::Option *imeiOpt = nullptr;
  CLI::Option *imsiOpt = nullptr;

  void initImeiOpt();
  void initImsiOpt();

  virtual void initOptions() override;

public:
  static std::unique_ptr<CLIParser> create();

  virtual Config redefineConfig(const Config &definedConfig) const override;
};
} // namespace client
