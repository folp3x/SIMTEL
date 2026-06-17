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
  std::string statusStr = ueActiveToStr(inActive);
  if (inActive) {
    common::printColored(statusStr, rang::fg::green, "");
  } else {
    common::printColored(statusStr, rang::fg::red, "");
  }

  std::cout << ", protocol: " << common::protocolToStr(protocol) << std::endl;
}

void Menu::showSignalInfo(const common::Location<> &location,
                          unsigned int signalLevel) const {
  std::string levelStr =
      (signalLevel > 0)
          ? std::to_string(signalLevel) + "/" +
                std::to_string(common::constants::MAX_SIGNAL_LEVEL)
          : "no signal";
  std::cout << "Location: " << location.toStr() << ", signal: " << levelStr
            << std::endl;
}

void Menu::showAddressBook(const std::map<char, common::msisdn_t> &book) const {
  std::cout << "Address book: " << std::endl;
  for (const auto &[speedDialNum, msisdn] : book) {
    std::cout << speedDialNum << " - " << msisdn << std::endl;
  }
}

void Menu::showSentSms(const common::Sms &sms) const {
  std::cout << "To " << sms.receiver << " ";
  common::printTime(sms.timeSent, " ");
  showSmsStatus(sms.delivered);
  std::cout << sms.content << std::endl;
}

void Menu::showReceivedSms(const common::Sms &sms) const {
  std::cout << "From " << sms.sender << " ";
  common::printTime(sms.timeReceived);
  std::cout << sms.content << std::endl;
}

void Menu::showError(const std::string &error) const {
  showMessage({error, common::MenuMessageType::ERR});
}

void Menu::showSmsStatus(bool delivered) const {
  if (delivered) {
    common::printColored("delivered", rang::fg::green);
  } else {
    common::printColored("pending", rang::fg::yellow);
  }
}
} // namespace client
