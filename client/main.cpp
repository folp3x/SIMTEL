#include "app/app/app.h"

#include <filesystem>

#include "app/cli/cli_parser/cli_parser.h"
#include "app/config/config_parser/config_parser.h"
#include "common/logging/logger/logger.h"

int main(int argc, char *argv[]) {
  try {
    // настройка логирования
    std::filesystem::create_directories("logs");
    try {
      common::Logger::initLogging("Client logger", "./logs", "client");
    } catch (const spdlog::spdlog_ex &e) {
      std::cerr << "Logger initialization error" << e.what() << std::endl;
    }

    // парсинг аргументов командной строки
    auto cliParser = client::CLIParser::create();

    std::string msg = "";
    bool helpCalled = false;
    bool parsed = cliParser->parse(argc, argv, msg, helpCalled);

    if (helpCalled) {
      // вызов --help
      std::cout << msg << std::endl;
      SPDLOG_LOGGER_CRITICAL(spdlog::default_logger(), "Help showed");
      return 0;
    }

    if (!parsed) {
      // ошибка парсинга
      std::cout << msg << std::endl;
      SPDLOG_LOGGER_CRITICAL(spdlog::default_logger(),
                             "Cli arg parse error: {}", msg);
      return 1;
    }

    client::Config config{};
    if (auto configFilePathParseResult = cliParser->getParsedConfigFilePath()) {
      // парсинг данных из конфигурационного файла --config
      std::string configFilePath = *configFilePathParseResult;
      auto configParser = client::ConfigParser::create();
      auto configParseResult = configParser->parse(configFilePath);
      if (!configParseResult) {
        std::cout << "Error parsing config file: " << configParseResult.error()
                  << std::endl;
        SPDLOG_LOGGER_CRITICAL(spdlog::default_logger(),
                               "Config file parse error: {}",
                               configParseResult.error());
        return 1;
      }

      config = *configParseResult;
    } else if (!cliParser->allConfigOptsSet()) {
      // если --config не указан должны быть указаны все конфигурационные опции
      std::cout
          << "If --config is not specified all config options are required"
          << std::endl;
      SPDLOG_LOGGER_CRITICAL(spdlog::default_logger(),
                             "--config or all config options not set");
      return 1;
    }

    // переопределение опций из файла опциями командной строки
    config = cliParser->redefineConfig(config);
    if (!config.isInitialized()) {
      std::cout << "Some config fields are not initialized" << std::endl;
      SPDLOG_LOGGER_CRITICAL(spdlog::default_logger(),
                             "--config or all config options not set");
      return 1;
    }

    common::Location location(config.getLoc());
    try {
      common::NetworkAddress addr{config.getIP(), config.getPort()};
      // запуск главного цикла приложения
      client::App app{location, addr, config.getImsi(), config.getImei()};
      app.run();
    } catch (std::invalid_argument &e) {
      std::cout << "Incorrect IP in config file" << std::endl;
      SPDLOG_LOGGER_CRITICAL(spdlog::default_logger(), "App run error: {}",
                             e.what());
      return 1;
    }

    return 0;
  } catch (std::exception &e) {
    std::cout << "Error occured:" << e.what() << std::endl;
    return 1;
  }
}
