#include "app/app/app.h"
#include "app/cli/cli_parser/cli_parser.h"
#include "app/config/config_parser/config_parser.h"
#include "common/logging/logger/logger.h"

int main(int argc, char *argv[]) {
  try {
    try {
      common::Logger::init("Server logger", "./logs", "server",
                           spdlog::level::debug);
    } catch (const spdlog::spdlog_ex &e) {
      std::cerr << "Logger initialization error" << e.what() << std::endl;
    }

    auto cliParser = server::CLIParser::create("server");

    std::string msg = "";
    bool helpCalled = false;
    bool parsed = cliParser->parse(argc, argv, msg, helpCalled);

    if (helpCalled) {
      std::cout << msg << std::endl;
      return 0;
    }

    if (!parsed) {
      std::cout << msg << std::endl;
      return 1;
    }

    server::Config config{};
    auto configFilePath = cliParser->getParsedConfigFilePath();
    if (configFilePath) {
      // парсинг данных из конфигурационного файла
      auto configParser = server::ConfigParser::create();
      auto parsedConfig = configParser->parse(*configFilePath);
      if (!parsedConfig) {
        std::cout << "Error parsing config file: " << parsedConfig.error()
                  << std::endl;
        return 1;
      }
      config = std::move(*parsedConfig);
    } else if (!cliParser->allConfigOptsSet()) {
      std::cout
          << "If --config is not specified all config options are required"
          << std::endl;
      return 1;
    }

    // переопределение опций из файла опциями командной строки
    config = cliParser->redefineConfig(config);
    if (!config.isInitialized()) {
      std::cout << "Some config fields are not initialized" << std::endl;
      return 1;
    }

    common::Location<> location(config.getLoc());
    common::NetworkAddress addr{"127.0.0.1", config.getPort()};

    server::App app{addr};
    app.run();

    return 0;
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    return 1;
  }
}
