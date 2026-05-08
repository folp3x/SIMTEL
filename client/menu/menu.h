#pragma once

#include "common/app/menu/menu.h"

#include <string>

#include "client/app_state.h"
#include "command_parser.h"
#include "common/core/location.h"
#include "common/core/protocol.h"
#include "common/types.h"

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
