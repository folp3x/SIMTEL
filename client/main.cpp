#include "app/app/app.h"
#include "app/cli/cli_parser/cli_parser.h"
#include "app/config/config_parser/config_parser.h"
#include "core/address_book/address_book_parser/address_book_parser.h"

int main(int argc, char *argv[]) {
  try {
    auto cliParser = client::CliParser::create();

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

    client::Config config{};
    auto filePath = cliParser->getConfigFilePath();
    if (filePath) {
      // парсинг данных из конфигурационного файла
      auto configParser = client::ConfigParser::create();
      auto parsedConfig = configParser->parse(*filePath);
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

    auto addressBookParser = client::AddressBookParser::create();
    auto parsedAddressBook =
        addressBookParser->parse(config.getAddressBookFilePath());

    std::map<char, common::msisdn_t> addressBook;
    if (parsedAddressBook) {
      addressBook = std::move(*parsedAddressBook);
    } else {
      std::cout << "Error loading address book: " << parsedAddressBook.error()
                << std::endl;
    }

    common::Location<> location(config.getLoc());
    common::NetworkAddress serverAddr{config.getIp(), config.getPort()};

    client::UeContext ctx{config.getImsi(), config.getImei(), location,
                          serverAddr};

    client::App app{ctx, addressBook};
    app.run();

    return 0;
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    return 1;
  }
}
