#include "app/app.h"

#include "app/cli/cli_parser.h"
#include "app/config/config_parser.h"

int main(int argc, char *argv[]) {
  CLIParser cliParser{};

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

  Config config{};
  if (auto configFilePathParseResult = cliParser.getParsedConfigFilePath()) {
    // парсинг данных из конфигурационного файла --config
    std::string configFilePath = *configFilePathParseResult;
    ConfigParser configParser{};
    auto configParseResult = configParser.parse(configFilePath);
    if (!configParseResult) {
      std::cout << "Error parsing file: " << configParseResult.error()
                << std::endl;
      return 1;
    }

    config = *configParseResult;
  } else if (!cliParser.allConfigOptsSet()) {
    // если --config не указан должны быть указаны все конфигурационные опции
    std::cout << "If --config is not specified all config options are required"
              << std::endl;
    return 1;
  }

  // переопределение опций из файла опциями командной строки
  if (auto ip = cliParser.getParsedIP())
    config.setIP(*ip);
  if (auto port = cliParser.getParsedPort())
    config.setPort(*port);
  if (auto imei = cliParser.getParsedImei())
    config.setImei(*imei);
  if (auto imsi = cliParser.getParsedImsi())
    config.setImsi(*imsi);
  if (auto loc = cliParser.getParsedLoc())
    config.setLoc(*loc);

  if (!config.isInitialized()) {
    std::cout << "Some config fields are not initialized" << std::endl;
    return 1;
  }

  // запуск главного цикла приложения
  App app(config);
  app.run();

  return 0;
}
