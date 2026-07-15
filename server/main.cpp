#include "app/app/app.h"
#include "app/cli_parser/cli_parser.h"

#include "app/config/bs_config/bs_config_parser/bs_config_parser.h"
#include "app/config/config_parser/config_parser.h"
#include "app/config/epc_config/epc_config_parser/epc_config_parser.h"

int main(int argc, char *argv[]) {
  try {
    auto cliParser = server::CliParser::create("server");

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
    auto filePath = cliParser->getConfigFilePath();
    if (!filePath) {
      std::cout << "Error: path to config file not specified" << std::endl;
      return 1;
    }

    auto configParser = server::ConfigParser::create();
    auto parsedConfig = configParser->parse(*filePath);
    if (!parsedConfig) {
      std::cout << "Error parsing main config file: " << parsedConfig.error()
                << std::endl;
      return 1;
    }

    config = cliParser->redefineConfig(*parsedConfig);

    auto bsConfigParser =
        server::BsConfigParser::create(config.getMmeConfigs());
    auto bsConfigs = bsConfigParser->parse(config.getBsFilePath());
    if (!bsConfigs) {
      std::cout << "Error parsing BS config file: " << bsConfigs.error()
                << std::endl;
      return 1;
    }

    auto epcConfigParser = server::EpcConfigParser::create();
    auto epcConfig = epcConfigParser->parse(config.getEpcFilePath());
    if (!epcConfig) {
      std::cout << "Error parsing EPC config file: " << epcConfig.error()
                << std::endl;
      return 1;
    }

    size_t maxUeThreadsCount = 0;
    for (const auto &config : *bsConfigs) {
      maxUeThreadsCount += config.maxConnections;
    }

    common::NetworkAddress addr{"127.0.0.1", config.getPort()};

    server::App app{addr,
                    maxUeThreadsCount,
                    config.getMmeConfigs(),
                    config.getSmscConfig(),
                    *bsConfigs,
                    *epcConfig,
                    config.getPcrfConfig()};
    app.run();

    return 0;
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    return 1;
  }
}
