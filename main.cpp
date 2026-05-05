#include "app/app.h"

#include "app/cli/cli_parser.h"
#include "app/config/config_parser.h"

int main(int argc, char *argv[]) {
  CLIParser cliParser;
  cliParser.setupOptions();

  std::string msg = "";
  bool helpCalled = false;
  bool parsed = cliParser.parse(argc, argv, msg, helpCalled);

  if (helpCalled) {
    // вызов --help
    std::cout << msg << std::endl;
    return 0;
  }

  if (!parsed) {
    // ошибка парсинга
    std::cout << msg << std::endl;
    return 1;
  }

  // должен быть указан либо --config либо любая другая опция
  if (!cliParser.hasConfig() && !cliParser.hasAnyNonConfig()) {
    std::cout << "Either --config or at least one other option is required"
              << std::endl;
    return 1;
  }

  // --config нельзя комбинировать с другими опциями
  if (cliParser.hasConfig() && cliParser.hasAnyNonConfig()) {
    std::cout << "Option --config cannot be combined with other options"
              << std::endl;
    return 1;
  }

  Config config;
  if (cliParser.hasConfig()) {
    // парсинг конфигурации из json
    std::string configFilePath = cliParser.getConfigFilePath();
    ConfigParser configParser;
    bool parsed = configParser.parse(configFilePath, msg, config);
    if (!parsed) {
      std::cout << "Error parsing file: " << msg << std::endl;
      return 1;
    }
  } else {
    config = cliParser.getConfig();
  }

  // запуск главного цикла приложения
  App app(config);
  app.run();

  return 0;
}
