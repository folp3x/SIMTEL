#include "cli_parser.h"

#include "app/validator/validator.h"

// задает аргументы командной строки и правила их проверки
void CLIParser::setupOptions() {
  ip_opt = cliApp.add_option("-a, --ip", config.ip, "Set IP address")
               ->check(Validator::isCorrectIP)
               ->type_name("IPv4");

  port_opt = cliApp.add_option("-p, --port", config.port, "Set port")
                 ->check([](const std::string &val) {
                   try {
                     int port = std::stoi(val);
                     return Validator::isCorrectPort(port);
                   } catch (const std::invalid_argument &e) {
                     return std::string("Port number must be integer");
                   }
                 })
                 ->type_name("integer");

  imei_opt = cliApp.add_option("-e, --imei", config.imei, "Set IMEI")
                 ->check(Validator::isCorrectIMEI)
                 ->type_name("char[15]");

  imsi_opt = cliApp.add_option("-i, --imsi", config.imsi, "Set IMSI")
                 ->check(Validator::isCorrectIMSI)
                 ->type_name("char[15]");

  loc_opt = cliApp.add_option("-l, --loc", config.loc, "Set position vector")
                ->type_name("x y z (real)");

  config_opt = cliApp
                   .add_option("-k, --config", configFilePath,
                               "Load config from specified config.json")
                   ->check(Validator::isCorrectConfigPath)
                   ->type_name("string");

  nodes_opt = cliApp
                  .add_option("-n, --nodes", nodesFilePath,
                              "Load nodes from specified nodes.json")
                  ->check(Validator::isCorrectNodesPath)
                  ->type_name("string");
}

// парсит аргументы, передает в msg сообщение об ошибке или help
bool CLIParser::parse(int argc, char *argv[], std::string &msg,
                      bool &helpCalled) {
  try {
    cliApp.parse(argc, argv);
    return true;
  } catch (const CLI::ParseError &e) {
    if (e.get_name() == "CallForHelp") {
      msg = cliApp.help();
    } else {
      msg = e.what();
    }
    return false;
  }
}

// проверяет указана ли опция --config
bool CLIParser::hasConfig() const {
  return config_opt && config_opt->count() > 0;
}

// проверяет указана ли любая опция кроме --config
bool CLIParser::hasAnyNonConfig() const {
  return (ip_opt && ip_opt->count() > 0) ||
         (port_opt && port_opt->count() > 0) ||
         (imei_opt && imei_opt->count() > 0) ||
         (imsi_opt && imsi_opt->count() > 0) ||
         (loc_opt && loc_opt->count() > 0) ||
         (nodes_opt && nodes_opt->count() > 0);
}

Config CLIParser::getConfig() const { return config; }

std::string CLIParser::getConfigFilePath() const { return configFilePath; }

std::string CLIParser::getNodesFilePath() const { return nodesFilePath; }
