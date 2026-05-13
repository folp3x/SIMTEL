#include "app.h"

#include <iostream>

#include "common/app/menu/menu_item/menu_item_exit/menu_item_exit.h"
#include "common/app/menu/menu_item/menu_item_invalid/menu_item_invalid.h"
#include "server/app/menu/menu/menu.h"
#include "server/app/menu/menu_item/menu_item_dist/menu_item_dist.h"
#include "server/core/distance_calculator/distance_calculator.h"

namespace server {
App::App(const common::Location<float> &location_,
         const common::NetworkAddress &addr_)
    : common::App<Config>(location_, addr_) {}

App::App(const App &other) : common::App<Config>(other.location, other.addr) {
  logConstructor("COPY", location, addr);
}

App::App(App &&other) noexcept
    : common::App<Config>(std::move(other.location), std::move(other.addr)) {
  logConstructor("MOVE", location, addr);
}

void App::logConstructor(const std::string constructorType,
                         const common::Location<float> &location,
                         const common::NetworkAddress &addr) const {
  SPDLOG_LOGGER_DEBUG(spdlog::default_logger(),
                      "client::App {} constructor called: location={}",
                      constructorType, location.toStr(), addr.toStr());
}

std::string App::handleCommand(const std::unique_ptr<common::MenuItem> &cmd,
                               bool &exit) {
  std::string message = "";
  // выполнение команды в засимости от ее типа
  if (auto *invalidCmd = dynamic_cast<common::MenuItemInvalid *>(cmd.get())) {
    message = "Error! " + invalidCmd->getError();
  } else if (dynamic_cast<common::MenuItemExit *>(cmd.get())) {
    message = "Exiting app...";
    exit = true;
  } else if (auto *distCmd = dynamic_cast<MenuItemDist *>(cmd.get())) {
    auto coords = distCmd->getCoords();
    float dist = DistanceCalculator::calc<std::vector<float>>(location, coords);
    message = "Distance: " + common::toStr<float>(dist, 4);
  }

  return message;
}

void App::run() {
  Menu menu;

  bool isRunning = true;

  while (isRunning) {
    menu.showStatus(location);
    menu.showCommandsInfo(getCommandsInfo());

    std::string extraMsg = "";
    auto cmd = menu.getCommand(extraMsg);

    if (!extraMsg.empty())
      menu.showMessage(extraMsg);

    bool exit = false;
    std::string message = handleCommand(cmd, exit);

    if (!message.empty())
      menu.showMessage(message);

    if (exit)
      isRunning = false;
    else
      std::cout << std::endl;
  }
}
} // namespace server
