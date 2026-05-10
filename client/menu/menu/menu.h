#pragma once

#include "common/app/menu/menu/menu.h"

#include <string>

#include "client/app_state/app_state.h"
#include "client/menu/command_parser/command_parser.h"
#include "common/core/location/location.h"
#include "common/network/protocol/protocol.h"
#include "common/types/types.h"

namespace client {
class Menu : public common::Menu<CommandParser> {
private:
  CommandParser parser{};

public:
  void showStatus(AppState state, const common::imsi_t &imsi,
                  const common::Location &location,
                  common::Protocol protocol) const;
  void showCommandsInfo() const;
};
} // namespace client
