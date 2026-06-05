#include "menu.h"

#include <iostream>

#include "common/logging/logger/logger.h"
#include "common/utils/print/print.h"

namespace client {
void Menu::logInput(const std::string &input) const {
  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                     "Received input: {}", input);
}

std::string Menu::getMessageContent() const {
  std::string content;
  std::string line;
  std::cout << "Write content (empty line means end of sms):" << std::endl;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      break;
    }
    content += line + "\n";
  }

  return content;
}

void Menu::showStatus(bool inActive, const common::imsi_t &imsi,
                      common::Protocol protocol) const {
  std::cout << "IMSI: " << imsi;

  // текущее состояние
  std::cout << ", state: ";
  std::string statusStr = appActiveToStr(inActive);
  if (inActive) {
    common::printColored(statusStr, rang::fg::green, "");
  } else {
    common::printColored(statusStr, rang::fg::red, "");
  }

  std::cout << ", protocol: " << common::protocolToStr(protocol) << std::endl;
}

void Menu::showSignalInfo(const common::Location<> &location,
                          std::optional<float> distance) const {
  std::string distStr = distance ? common::toStr(*distance) : "unknown";
  std::cout << "Location: " << location.toStr()
            << ", distance to BS: " << distStr << std::endl;
}

void Menu::showAddressBook(const std::map<char, common::msisdn_t> &book) const {
  std::cout << "Address book: " << std::endl;
  for (const auto &[speedDialNum, msisdn] : book) {
    std::cout << speedDialNum << " - " << msisdn << std::endl;
  }
}
} // namespace client
