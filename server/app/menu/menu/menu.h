#pragma once

#include "common/app/menu/menu/menu.h"

#include "common/core/location/location/location.h"
#include "server/app/menu/command_parser/command_parser.h"

namespace server {
class Menu : public common::Menu<CommandParser> {
private:
  CommandParser parser{};

  virtual void logInput(const std::string &input) const override;

public:
  void showStatus(const common::Location<> &location) const;

  void showSocketActive(bool isActive) const;
};
} // namespace server
