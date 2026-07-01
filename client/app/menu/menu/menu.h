#pragma once

#include "common/app/menu/interactive_menu/interactive_menu.h"

#include "client/app/menu/command_parser/command_parser.h"
#include "client/core/sms/sms/sms.h"
#include "common/core/location/location/location.h"
#include "common/network/network_address/network_address.h"
#include "common/network/protocol/protocol.h"

namespace client {
class Menu : public common::InteractiveMenu<CommandParser> {
private:
  CommandParser parser{};

  rang::fg getSmsStatusColor(SmsStatus status) const;

public:
  std::string getMessageContent() const;

  void showStatus(bool inActive, const common::imsi_t &imsi,
                  common::Protocol protocol) const;

  void showSignalInfo(const common::Location<> &location,
                      unsigned int signalLevel) const;

  void showAddressBook(const std::map<char, common::msisdn_t> &book) const;

  void showSentSms(const Sms &sms, bool alignRight = false) const;
  void showReceivedSms(const Sms &sms) const;

  void showError(const std::string &error) const;
};
} // namespace client
