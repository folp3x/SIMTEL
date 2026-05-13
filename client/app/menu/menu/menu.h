#pragma once

#include "common/app/menu/menu/menu.h"

#include "client/app/app_state/app_state.h"
#include "client/app/menu/command_parser/command_parser.h"
#include "common/core/location/location.h"
#include "common/network/protocol/protocol.h"
#include "common/types/types.h"

namespace client {
class Menu : public common::Menu<CommandParser> {
private:
  CommandParser parser{};

  virtual void logInput(const std::string &input) const override;

public:
  void showStatus(AppState state, const common::imsi_t &imsi,
                  const common::Location<float> &location,
                  common::Protocol protocol) const;
};
} // namespace client
