#include "app.h"

#include <iostream>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <variant>

#include "client/app/menu/menu/menu.h"

namespace client {
std::string App::handleActiveCommand(const MenuItemActive &cmd) {
  logCommandProcess(cmd.getName(), fmt::format("active={}", cmd.getActive()));
  AppState newState = cmd.getActive() ? AppState::ACTIVE : AppState::INACTIVE;

  bool stateChanged = newState != state;
  if (stateChanged)
    state = newState;

  std::string stateStr = std::string(appStateToStr(newState));
  return formChangeMessage("State", stateStr, stateChanged);
}

std::string App::handleMoveCommand(const MenuItemMove &cmd) {
  std::vector<float> coords = cmd.getCoords();
  logCommandProcess(
      cmd.getName(),
      fmt::format("coords={}", common::toStr(coords.begin(), coords.end())));
  try {
    bool locationChanged = !location.coordsEqual(coords);
    if (locationChanged)
      location.move<std::vector<float>>(coords);

    return formChangeMessage("Position", location.toStr(), locationChanged);
  } catch (const std::invalid_argument &e) {
    return "Position coords count is invalid";
  }
}

std::string App::handleProtocolCommand(const MenuItemProtocol &cmd) {
  std::string protocolStr = cmd.getProtocol();
  logCommandProcess(cmd.getName(), fmt::format("protocol={}", protocolStr));

  auto protocolParseResult = common::protocolFromStr(protocolStr);
  if (protocolParseResult) {
    common::Protocol newProtocol = *protocolParseResult;

    bool protocolChanged = newProtocol != protocol;
    if (protocolChanged)
      protocol = newProtocol;

    std::string protocolStr = std::string(protocolToStr(newProtocol));
    return formChangeMessage("Protocol", protocolStr, protocolChanged);
  }
  return "Invalid protocol";
}

std::string App::handleCommand(const std::unique_ptr<common::MenuItem> &cmd,
                               bool &exit) {
  std::string message = "";

  std::string cmdNameUpper = common::uppercased(cmd->getName());

  bool correctCommand = true;
  // выполнение команды в засимости от ее типа
  if (dynamic_cast<MenuItemUnknown *>(cmd.get())) {
    SPDLOG_LOGGER_INFO(spdlog::default_logger(), "Received unknown command");
    message = "Unknown command";
    correctCommand = false;
  } else if (auto *invalidCmd = dynamic_cast<MenuItemInvalid *>(cmd.get())) {
    SPDLOG_LOGGER_INFO(spdlog::default_logger(), "Received invalid command: {}",
                       invalidCmd->getError());
    message = "Error! " + invalidCmd->getError();
    correctCommand = false;
  } else if (dynamic_cast<MenuItemExit *>(cmd.get())) {
    logCommandProcess(cmdNameUpper);
    message = "Exiting app...";
    exit = true;
  } else if (auto *activeCmd = dynamic_cast<MenuItemActive *>(cmd.get())) {
    message = handleActiveCommand(*activeCmd);
  } else if (auto *moveCmd = dynamic_cast<MenuItemMove *>(cmd.get())) {
    message = handleMoveCommand(*moveCmd);
  } else if (auto *protocolCmd = dynamic_cast<MenuItemProtocol *>(cmd.get())) {
    message = handleProtocolCommand(*protocolCmd);
  }

  if (correctCommand) {
    SPDLOG_LOGGER_INFO(spdlog::default_logger(), "Finished command {}",
                       cmdNameUpper);
  }

  return message;
}

App::App(const common::Location &location_, const common::NetworkAddress &addr_,
         const common::imsi_t &imsi_, const common::imei_t imei_)
    : common::App<Config>(location_, addr_), imsi(imsi_), imei(imei_) {}

// получает команды от пользователя через меню и выполняет их
void App::run() {
  Menu menu;

  bool isRunning = true;

  SPDLOG_LOGGER_INFO(spdlog::default_logger(), "App started");
  while (isRunning) {
    menu.showStatus(state, imsi, location, protocol);
    menu.showCommandsInfo();

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
  SPDLOG_LOGGER_INFO(spdlog::default_logger(), "App exited");
}
} // namespace client
