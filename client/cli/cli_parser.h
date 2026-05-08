#pragma once

#include "common/app/cli/cli_parser.h"

#include "client/config/config.h"

namespace client {
class CLIParser : public common::CLIParser<Config> {
private:
  explicit CLIParser(const std::string &cliAppName);

  std::string nodesFilePath = "";

  // опции для получения параметров конфига
  CLI::Option *imeiOpt = nullptr;
  CLI::Option *imsiOpt = nullptr;

  CLI::Option *nodesFileOpt = nullptr;

  void initImeiOpt();
  void initImsiOpt();
  void initNodesFileOpt();

  virtual void initOptions() override;

public:
  static std::unique_ptr<CLIParser> create();

  std::optional<std::string> getParsedNodesFilePath() const;

  virtual Config redefineConfig(const Config &definedConfig) const override;
};
} // namespace client
