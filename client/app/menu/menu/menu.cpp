#include "menu.h"

#include <iostream>

#include "client/core/ue/ue_active/ue_active.h"
#include "common/utils/print/print.h"

namespace client {
rang::fg Menu::getSmsStatusColor(SmsStatus status) const {
  switch (status) {
  case SmsStatus::PENDING:
    return rang::fg::yellow;
  case SmsStatus::DELIVERED:
    return rang::fg::green;
  case SmsStatus::NOT_DELIVERED:
    return rang::fg::red;
  default:
    return rang::fg::reset;
  }
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

void Menu::showSentSms(const Sms &sms, bool alignRight) const {
  std::string leftHeaderPart =
      "To " + sms.receiver + " at " + common::formatTime(sms.timeSent) + " ";
  std::string statusStr = "(" + smsStatusToStr(sms.status) + ")";
  std::string headerEnding = ":";

  size_t fullHeaderLength =
      leftHeaderPart.size() + statusStr.size() + headerEnding.size();

  size_t headerLeftPadding =
      alignRight ? MENU_HEADER_LINE_LENGTH - fullHeaderLength : 0;
  std::string headerLeftSpace = std::string(headerLeftPadding, ' ');

  std::cout << headerLeftSpace << leftHeaderPart;
  common::printColored(statusStr, getSmsStatusColor(sms.status), "");
  std::cout << headerEnding << std::endl;

  if (alignRight) {
    std::cout << std::right << std::setw(MENU_HEADER_LINE_LENGTH)
              << sms.content;
  } else {
    std::cout << sms.content;
  }

  std::cout << std::endl;
}

void Menu::showReceivedSms(const Sms &sms) const {
  std::cout << "From " << sms.sender << " at ";
  std::cout << common::formatTime(sms.timeReceived) << ":" << std::endl;
  std::cout << sms.content << std::endl;
}

void Menu::showError(const std::string &error) const {
  showMessage({error, common::MenuMessageType::ERR});
}
} // namespace client
