#pragma once

#include "common/app/menu/interactive_menu/interactive_menu.h"

#include "app/address_book/address_book.h"
#include "app/menu/command_parser/command_parser.h"
#include "app/menu/ussd_info/ussd_info.h"

#include "common/core/location/location/location.h"
#include "core/sms/sms/sms.h"

#include "common/network/network_address/network_address.h"
#include "common/network/protocol/protocol.h"

namespace client {
class Menu : public common::InteractiveMenu<CommandParser> {
private:
  CommandParser parser{};

  static rang::fg getSmsStatusColor(SmsStatus status);

  static void showSmsContent(const std::string &content, bool alignRight);

public:
  std::string getSmsContent() const;

  void showStatus(bool inActive, const common::imsi_t &imsi,
                  common::Protocol protocol) const;

  void showSignalInfo(const common::Location<> &location,
                      unsigned int signalLevel) const;

  void showAddressBook(const AddressBook &addressBook) const;

  void showSentSms(const Sms &sms, bool alignRight = false) const;
  void showReceivedSms(const Sms &sms) const;

  void showError(const std::string &error) const;

  void showUssdInfo(const std::vector<UssdInfo> &info) const;
};
} // namespace client
