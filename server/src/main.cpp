#include "app/app/app.h"
#include "app/cli_parser/cli_parser.h"

#include "app/config/bs_config/bs_config_list/bs_config_list.h"
#include <string>

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

    auto filePath = cliParser->getConfigFilePath();
    if (!filePath) {
      std::cout << "Error: path to config file not specified" << std::endl;
      return 1;
    }

    server::Config config;
    auto configError = config.fromJsonFile(*filePath);
    if (configError) {
      std::cout << "Error parsing main config file: " << *configError
                << std::endl;
      return 1;
    }

    server::BsConfigList bsConfigList{config.getMmeConfigs()};
    auto bsConfigError = bsConfigList.fromJsonFile(config.getBsFilePath());
    if (bsConfigError) {
      std::cout << "Error parsing BS config file: " << *bsConfigError
                << std::endl;
      return 1;
    }

    server::EpcConfig epcConfig;
    auto epcConfigError = epcConfig.fromJsonFile(config.getEpcFilePath());
    if (epcConfigError) {
      std::cout << "Error parsing EPC config file: " << *epcConfigError
                << std::endl;
      return 1;
    }

    auto bsConfigs = bsConfigList.getConfigs();
    size_t maxUeThreadsCount = 0;
    for (const auto &config : bsConfigs) {
      maxUeThreadsCount += config.maxConnections;
    }

    common::NetworkAddress addr{"127.0.0.1", config.getPort()};

    server::App app{addr,
                    maxUeThreadsCount,
                    config.getMmeConfigs(),
                    config.getSmscConfig(),
                    bsConfigs,
                    epcConfig,
                    config.getPcrfConfig()};
    app.run();

    return 0;
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    return 1;
  }
}
