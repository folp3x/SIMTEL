#include "cli_parser.h"

#include "common/validator/validator.h"

namespace server {
CLIParser::CLIParser(const std::string &cliAppName)
    : common::CLIParser<Config>(cliAppName) {}

void CLIParser::initNodesFileOpt() {
  nodesFileOpt = cliApp.add_option_function<std::string>(
      "-n, --nodes",
      [this](const std::string &filePath) { nodesFilePath = filePath; },
      "Load nodes from specified JSON file");
  nodesFileOpt->check(common::Validator::isCorrectNodesPath);
  nodesFileOpt->type_name("string");
}

void CLIParser::initOptions() {
  common::CLIParser<Config>::initOptions();
  initNodesFileOpt();
}

std::optional<std::string> CLIParser::getParsedNodesFilePath() const {
  if (isOptSet(nodesFileOpt)) {
    return nodesFilePath;
  }
  return std::nullopt;
}

std::unique_ptr<CLIParser> CLIParser::create() {
  auto parser = std::unique_ptr<CLIParser>(new CLIParser("Server"));
  parser->initOptions();
  return parser;
}
} // namespace server
