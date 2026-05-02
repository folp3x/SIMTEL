#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <CLI/CLI.hpp>
#include <optional>
#include <vector>

#include "app/config/config.h"
#include "common/constants.h"

class CLIParser {
private:
  CLI::App cliApp{Constants::APP_TITLE};

  Config config{};

  // опции для получения путей к файлам
  std::string configFilePath = "";
  std::string nodesFilePath = "";

  // опции для получения параметров конфига
  CLI::Option *configOpt = nullptr;
  CLI::Option *ipOpt = nullptr;
  CLI::Option *portOpt = nullptr;
  CLI::Option *imeiOpt = nullptr;
  CLI::Option *imsiOpt = nullptr;
  CLI::Option *locOpt = nullptr;

  std::vector<CLI::Option *> configOpts = {};

  // опции для получения путей к файлам
  CLI::Option *configFileOpt = nullptr;
  CLI::Option *nodesFileOpt = nullptr;

  void initOptions();

  static bool isOptSet(CLI::Option *opt);

public:
  CLIParser();

  bool parse(int argc, char *argv[], std::string &msg, bool &helpCalled);

  bool allConfigOptsSet() const;

  std::optional<std::string> getParsedIP() const;
  std::optional<int> getParsedPort() const;
  std::optional<std::string> getParsedImei() const;
  std::optional<std::string> getParsedImsi() const;
  std::optional<std::array<double, Constants::LOCATION_COORDS_COUNT>>
  getParsedLoc() const;
  std::optional<std::string> getParsedConfigFilePath() const;
  std::optional<std::string> getParsedNodesFilePath() const;
};

#endif // CLI_PARSER_H
