#pragma once

#include "common/app/cli/cli_parser/cli_parser.h"

#include "server/config/config/config.h"

namespace server {
class CLIParser : public common::CLIParser<Config> {
private:
  explicit CLIParser(const std::string &cliAppName);

  std::string nodesFilePath = "";

  CLI::Option *nodesFileOpt = nullptr;

  void initNodesFileOpt();

  virtual void initOptions() override;

public:
  static std::unique_ptr<CLIParser> create();

  std::optional<std::string> getParsedNodesFilePath() const;
};
} // namespace server
