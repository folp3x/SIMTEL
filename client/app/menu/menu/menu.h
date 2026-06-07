#pragma once

#include "common/app/menu/menu/menu.h"

#include <optional>

#include "client/app/menu/command_parser/command_parser.h"
#include "client/core/ue/ue_active/ue_active.h"
#include "common/core/location/location/location.h"
#include "common/network/network_address/network_address.h"
#include "common/network/protocol/protocol.h"
#include "common/types.h"

namespace client {
class Menu : public common::Menu<CommandParser> {
private:
  CommandParser parser{};

  virtual void logInput(const std::string &input) const override;

public:
  std::string getMessageContent() const;

  void showStatus(bool inActive, const common::imsi_t &imsi,
                  common::Protocol protocol) const;

  void showSignalInfo(const common::Location<> &location,
                      std::optional<float> distance) const;

  void showAddressBook(const std::map<char, common::msisdn_t> &book) const;
};
} // namespace client
