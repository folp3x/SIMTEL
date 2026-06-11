#include "app/app/app.h"
#include "app/cli/cli_parser/cli_parser.h"
#include "app/config/config_parser/config_parser.h"
#include "common/logging/logger/logger.h"
#include "core/address_book/address_book_parser/address_book_parser.h"

int main(int argc, char *argv[]) {
  try {
    try {
      common::Logger::init("Client logger", "./logs", "client",
                           spdlog::level::debug);
    } catch (const spdlog::spdlog_ex &e) {
      std::cerr << "Logger initialization error" << e.what() << std::endl;
    }

    auto cliParser = client::CLIParser::create();

    std::string msg = "";
    bool helpCalled = false;
    bool parsed = cliParser->parse(argc, argv, msg, helpCalled);

    if (helpCalled) {
      std::cout << msg << std::endl;
      SPDLOG_LOGGER_CRITICAL(common::Logger::instance().getInner(),
                             "Help showed");
      return 0;
    }

    if (!parsed) {
      std::cout << msg << std::endl;
      SPDLOG_LOGGER_CRITICAL(common::Logger::instance().getInner(),
                             "Cli arg parse error: {}", msg);
      return 1;
    }

    client::Config config{};
    auto configFilePath = cliParser->getParsedConfigFilePath();
    if (configFilePath) {
      // парсинг данных из конфигурационного файла
      auto configParser = client::ConfigParser::create();
      auto parsedConfig = configParser->parse(*configFilePath);
      if (!parsedConfig) {
        std::cout << "Error parsing config file: " << parsedConfig.error()
                  << std::endl;
        SPDLOG_LOGGER_CRITICAL(common::Logger::instance().getInner(),
                               "Config file parse error: {}",
                               parsedConfig.error());
        return 1;
      }

      config = std::move(*parsedConfig);
    } else if (!cliParser->allConfigOptsSet()) {
      std::cout
          << "If --config is not specified all config options are required"
          << std::endl;
      SPDLOG_LOGGER_CRITICAL(common::Logger::instance().getInner(),
                             "--config or all config options not set");
      return 1;
    }

    // переопределение опций из файла опциями командной строки
    config = cliParser->redefineConfig(config);
    if (!config.isInitialized()) {
      std::cout << "Some config fields are not initialized" << std::endl;
      SPDLOG_LOGGER_CRITICAL(common::Logger::instance().getInner(),
                             "--config or all config options not set");
      return 1;
    }

    auto addressBookParser = client::AddressBookParser::create();
    auto addressBook =
        addressBookParser->parse(config.getAddressBookFilePath());

    if (addressBook) {
    } else {
      std::cout << "Error loading address book: " << addressBook.error()
                << std::endl;
    }

    common::Location<> location(config.getLoc());
    common::NetworkAddress serverAddr{config.getIP(), config.getPort()};

    client::UeContext ctx{config.getImsi(), config.getImei(), location,
                          serverAddr};

    client::App app{ctx, *addressBook};
    app.run();

    return 0;
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    return 1;
  }
}
