#include "menu.h"

#include <iostream>
#include <spdlog/spdlog.h>

#include "common/types/types.h"
#include "common/utils/print/print.h"

namespace client {
void Menu::logInput(const std::string &input) const {
  SPDLOG_LOGGER_INFO(spdlog::default_logger(), "Received input: {}", input);
}

void Menu::showStatus(AppState state, const common::imsi_t &imsi,
                      const common::Location<float> &location,
                      common::Protocol protocol) const {
  showHeaderLine();

  std::cout << "IMSI: " << imsi << std::endl;

  // текущее состояние
  std::cout << "State: ";
  std::string_view statusStr = appStateToStr(state);
  switch (state) {
  case AppState::ACTIVE:
    common::printColored(statusStr, rang::fg::green);
    break;
  case AppState::INACTIVE:
    common::printColored(statusStr, rang::fg::red);
    break;
  }

  std::cout << "Location: " << location.toStr() << std::endl;
  std::cout << "Protocol: " << common::protocolToStr(protocol) << std::endl;

  showHeaderLine();
}
} // namespace client
