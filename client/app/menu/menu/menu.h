#pragma once

#include "common/app/menu/menu/menu.h"

#include <optional>

#include "client/app/app_active/app_active.h"
#include "client/app/menu/command_parser/command_parser.h"
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
  void showStatus(bool inActive, const common::imsi_t &imsi,
                  const common::Location<> &location,
                  common::Protocol protocol) const;

  void showDistance(const common::NetworkAddress &serverAddr,
                    std::optional<float>) const;
};
} // namespace client
