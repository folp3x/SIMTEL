#include "app/app/app.h"
#include "app/cli/cli_parser/cli_parser.h"
#include "app/config/bs_config/bs_config_parser/bs_config_parser.h"
#include "app/config/config_parser/config_parser.h"
#include "app/config/epc_config/epc_config_parser/epc_config_parser.h"
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
        std::cout << "Error parsing main config file: " << parsedConfig.error()
                  << std::endl;
        return 1;
      }
      config = std::move(*parsedConfig);
    } else {
      std::cout << "Error: path to config file not specified" << std::endl;
      return 1;
    }

    // переопределение опций из файла опциями командной строки
    config = cliParser->redefineConfig(config);

    auto bsConfigParser =
        server::BsConfigParser::create(config.getMmeConfigs());
    auto parsedBsConfigs = bsConfigParser->parse(config.getBsFilePath());
    if (!parsedBsConfigs) {
      std::cout << "Error parsing BS config file: " << parsedBsConfigs.error()
                << std::endl;
      return 1;
    }

    auto epcConfigParser = server::EpcConfigParser::create();
    auto parsedEpcConfig = epcConfigParser->parse(config.getEpcFilePath());
    if (!parsedEpcConfig) {
      std::cout << "Error parsing EPC config file: " << parsedEpcConfig.error()
                << std::endl;
      return 1;
    }

    common::NetworkAddress addr{"127.0.0.1", config.getPort()};

    size_t maxUeThreadsCount = 0;
    for (const auto &config : *parsedBsConfigs) {
      maxUeThreadsCount += config.maxConnections;
    }

    server::App app{addr,
                    maxUeThreadsCount,
                    config.getMmeConfigs(),
                    config.getSmscConfig(),
                    *parsedBsConfigs,
                    *parsedEpcConfig};
    app.run();

    return 0;
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    return 1;
  }
}
