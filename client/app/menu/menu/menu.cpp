#include "menu.h"

#include <iostream>

#include "common/logging/logger/logger.h"
#include "common/utils/print/print.h"

namespace client {
void Menu::logInput(const std::string &input) const {
  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                     "Received input: {}", input);
}

void Menu::showStatus(bool inActive, const common::imsi_t &imsi,
                      const common::Location<> &location,
                      common::Protocol protocol) const {
  std::cout << "IMSI: " << imsi << std::endl;

  // текущее состояние
  std::cout << "State: ";
  std::string statusStr = appActiveToStr(inActive);
  if (inActive) {
    common::printColored(statusStr, rang::fg::green);
  } else {
    common::printColored(statusStr, rang::fg::red);
  }

  std::cout << "Location: " << location.toStr() << std::endl;
  std::cout << "Protocol: " << common::protocolToStr(protocol) << std::endl;
}

void Menu::showDistance(const common::NetworkAddress &serverAddr,
                        std::optional<float> distance) const {
  std::string serverAddrStr = serverAddr.toStr();
  std::string distStr = distance ? common::toStr(*distance) : "unknown";
  std::cout << "Distance to " + serverAddrStr + ": " + distStr << std::endl;
}

void Menu::showAddressBook(const std::vector<AddressBookRecord> &book) const {
  std::cout << "Address book: " << std::endl;
  for (const auto &record : book) {
    std::cout << record.speedDialNum << " - " << record.msisdn << std::endl;
  }
}
} // namespace client
