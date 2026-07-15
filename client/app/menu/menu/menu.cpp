#include "menu.h"

#include <iostream>

#include "client/core/ue/ue_active/ue_active.h"
#include "client/utils/str.h"
#include "common/utils/num/num.h"
#include "common/utils/print/print.h"
#include "common/utils/time/time.h"

namespace client {
rang::fg Menu::getSmsStatusColor(SmsStatus status) const {
  switch (status) {
  case SmsStatus::Pending:
    return rang::fg::yellow;
  case SmsStatus::Delivered:
    return rang::fg::green;
  case SmsStatus::NotDelivered:
    return rang::fg::red;
  default:
    return rang::fg::reset;
  }
}

std::string Menu::getSmsContent() const {
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
    common::utils::printColored(statusStr, rang::fg::green, "");
  } else {
    common::utils::printColored(statusStr, rang::fg::red, "");
  }

  std::cout << ", protocol: " << common::protocolToStr(protocol) << std::endl;
}

void Menu::showSignalInfo(const common::Location<> &location,
                          unsigned int signalLevel) const {
  std::string levelStr =
      (signalLevel > 0) ? std::to_string(signalLevel) + "/" +
                              std::to_string(common::constants::MaxSignalLevel)
                        : "no signal";
  std::cout << "Location: " << location.toStr() << ", signal: " << levelStr
            << std::endl;
}

void Menu::showAddressBook(const AddressBook &addressBook) const {
  std::cout << "Address book: ";
  auto records = addressBook.getRecords();
  if (records.empty()) {
    std::cout << "empty" << std::endl;
  } else {
    std::cout << std::endl;
    for (const auto &[speedDialNum, msisdn] : records) {
      std::cout << speedDialNum << " - " << msisdn << std::endl;
    }
  }
}

void Menu::showSentSms(const Sms &sms, bool alignRight) const {
  std::string leftHeaderPart =
      "To " + sms.receiver + " at " + common::utils::formatTime(sms.timeSent);
  std::string statusStr = " " + utils::smsStatusToStr(sms.status);
  std::string headerEnding = ":";

  size_t fullHeaderLength =
      leftHeaderPart.size() + statusStr.size() + headerEnding.size();

  size_t headerLeftPadding =
      alignRight ? MenuHeaderLineLength - fullHeaderLength : 0;
  std::string headerLeftSpace = std::string(headerLeftPadding, ' ');

  std::cout << headerLeftSpace << leftHeaderPart;
  if (sms.status != SmsStatus::Pending) {
    common::utils::printColored(statusStr, getSmsStatusColor(sms.status), "");
  }
  std::cout << headerEnding << std::endl;

  if (!alignRight) {
    std::cout << sms.content << std::endl;
  } else {
    std::istringstream stream(sms.content);
    std::string curLine;

    while (std::getline(stream, curLine)) {
      std::cout << std::right << std::setw(MenuHeaderLineLength) << curLine
                << std::endl;
    }
  }
}

void Menu::showReceivedSms(const Sms &sms) const {
  std::cout << "From " << sms.sender << " at ";
  std::cout << common::utils::formatTime(sms.timeReceived) << ":" << std::endl;
  std::cout << sms.content << std::endl;
}

void Menu::showError(const std::string &error) const {
  showMessage({error, common::MenuMessageType::Error});
}

void Menu::showUssdInfo(const std::vector<UssdInfo> &info) const {
  for (const auto &ussd : info) {
    std::cout << std::to_string(common::utils::ussdCodeToNum(ussd.code))
              << " - " << ussd.description << std::endl;
  }
}
} // namespace client
