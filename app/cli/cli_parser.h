#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <CLI/CLI.hpp>

#include "app/config/config.h"

class CLIParser {
private:
  CLI::App cliApp{"Console application"};
  Config config;
  std::string configFilePath = "";
  std::string nodesFilePath = "";

  // список опций
  CLI::Option *config_opt = nullptr;
  CLI::Option *ip_opt = nullptr;
  CLI::Option *port_opt = nullptr;
  CLI::Option *imei_opt = nullptr;
  CLI::Option *imsi_opt = nullptr;
  CLI::Option *loc_opt = nullptr;
  CLI::Option *nodes_opt = nullptr;

public:
  void setupOptions();
  bool parse(int argc, char *argv[], std::string &msg, bool &helpCalled);

  bool hasConfig() const;
  bool hasAnyNonConfig() const;

  Config getConfig() const;
  std::string getConfigFilePath() const;
  std::string getNodesFilePath() const;
};

#endif // CLI_PARSER_H
