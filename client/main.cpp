#include "app/app/app.h"
#include "app/cli_parser/cli_parser.h"
#include "app/locale/locale.h"

int main(int argc, char *argv[]) {
  try {
    const char *AppName = "client";

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

    const char *DefaultLocale = "en_EN.UTF-8";
    auto localeAlias = cliParser->getLocaleAlias();

    if (!localeAlias) {
      setlocale(LC_ALL, DefaultLocale);
    } else {
      auto locale = client::localeNameFromAlias(*localeAlias);
      if (!locale) {
        std::cout << "Unsupported locale" << std::endl;
        setlocale(LC_ALL, DefaultLocale);
      } else {
        setlocale(LC_ALL, locale->c_str());
        bindtextdomain(AppName, "./data/translations");
        textdomain(AppName);
      }
    }

    client::Config config{};
    auto filePath = cliParser->getConfigFilePath();
    if (filePath) {
      auto error = config.fromJsonFile(*filePath);
      if (error) {
        std::cout << _("Error parsing config file") << ": " << *error
                  << std::endl;
        return 1;
      }
    } else if (!cliParser->allConfigOptsSet()) {
      std::cout
          << _("If --config is not specified all config options are required")
          << std::endl;
      return 1;
    }

    // переопределение опций из файла опциями командной строки
    config = cliParser->redefineConfig(config);

    client::AddressBook addressBook{};
    auto error = addressBook.fromJsonFile(config.getAddressBookFilePath());
    if (error) {
      std::cout << _("Error while loading address book") << ": " << *error
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
