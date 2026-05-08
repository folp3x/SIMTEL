#include "menu.h"

#include <iostream>
#include <spdlog/spdlog.h>

#include "common/types.h"
#include "common/utils/print.h"

namespace client {
// выводит текущий статус
void Menu::showStatus(AppState state, const common::imsi_t &imsi,
                      const common::Location &location,
                      common::Protocol protocol) const {
  showHeaderLine();

  std::cout << "IMSI: " << imsi << std::endl;

  // текущее состояние
  std::cout << "State: ";
  std::string statusStr = appStateToStr(state);
  switch (state) {
  case AppState::ACTIVE:
    common::printlnColored(statusStr, rang::fg::green);
    break;
  case AppState::INACTIVE:
    common::printlnColored(statusStr, rang::fg::red);
    break;
  }

  std::cout << "Location: " << location.toStr() << std::endl;
  std::cout << "Protocol: " << common::protocolToStr(protocol) << std::endl;

  showHeaderLine();
}

// выводит информацию о доступных командах
void Menu::showCommandsInfo() const {
  const auto &commands = getCommandsInfo();
  for (const auto &[_, info] : commands) {
    std::cout << "- " << info.usage << " - " << info.description << std::endl;
  }
}
} // namespace client
