#pragma once

#include "common/app/menu/menu/menu.h"

#include "common/core/location/location.h"
#include "server/app/menu/command_parser/command_parser.h"

namespace server {
class Menu : public common::Menu<CommandParser> {
private:
  CommandParser parser{};

public:
  void showStatus(const common::Location<float> &location) const;
};
} // namespace server
