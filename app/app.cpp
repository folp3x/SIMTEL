#include "app.h"

#include <iostream>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <variant>

#include "menu/menu.h"
#include "utils/str.h"
#include "utils/vec.h"

// выполняет команду 'active', возвращает сообщение
std::string App::handleActiveCommand(const MenuItemActive &cmd) {
  SPDLOG_LOGGER_INFO(spdlog::default_logger(),
                     "Processing command {} with args: active={}",
                     uppercased(cmd.getName()), cmd.getActive());
  AppState newState = cmd.getActive() ? AppState::ACTIVE : AppState::INACTIVE;

  std::string stateStr = appStateToStr(newState);
  if (newState != state) {
    state = newState;
    return "Status changed to " + stateStr;
  }
  return "Status already set to " + stateStr;
}

// выполняет команду 'move', возвращает сообщение
std::string App::handleMoveCommand(const MenuItemMove &cmd) {
  SPDLOG_LOGGER_INFO(spdlog::default_logger(),
                     "Processing command {} with args: coords={}",
                     uppercased(cmd.getName()), toStr(cmd.getCoords()));

  try {
    if (!location.coordsEqual(cmd.getCoords())) {
      location.move(cmd.getCoords());
      return "Position changed to " + location.toStr();
    }
    return "Position already set to " + location.toStr();
  } catch (const std::invalid_argument &e) {
    return "Position coords count is invalid";
  }
}

// выполняет команду 'protocol', возвращает сообщение
std::string App::handleProtocolCommand(const MenuItemProtocol &cmd) {
  SPDLOG_LOGGER_INFO(spdlog::default_logger(),
                     "Processing command {} with args: protocol={}",
                     uppercased(cmd.getName()), cmd.getProtocol());

  auto protocolParseResult = protocolFromStr(cmd.getProtocol());
  if (protocolParseResult) {
    Protocol newProtocol = *protocolParseResult;
    std::string protocolStr = protocolToStr(newProtocol);
    if (newProtocol != protocol) {
      protocol = newProtocol;
      return "Protocol changed to " + protocolStr;
    }
    return "Protocol already set to " + protocolStr;
  }
  return "Ivalid protocol";
}

std::string App::handleCommand(const std::unique_ptr<MenuItem> &cmd,
                               bool &exit) {
  std::string message = "";

  std::string cmdNameUpper = uppercased(cmd->getName());

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
    SPDLOG_LOGGER_INFO(spdlog::default_logger(), "Processing command {}",
                       cmdNameUpper);
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

App::App(const Config &config)
    : addr(config.getIP(), config.getPort()), imei(config.getImei()),
      imsi(config.getImsi()), location(config.getLoc()) {}

App::App(const std::string &imsi_, const Location &location_,
         const NetworkAddress &addr_, const std::string imei_)
    : imsi(imsi_), location(location_), addr(addr_), imei(imei_) {}

// получает команды от пользователя через меню и выполняет их
void App::run() {
  Menu menu;
  // сообщение с результатами выполнения команды
  std::string message = "";

  bool isRunning = true;

  SPDLOG_LOGGER_INFO(spdlog::default_logger(), "App started");
  while (isRunning) {
    menu.showStatus(state, imsi, location, protocol);
    menu.showCommandsInfo();

    std::unique_ptr<MenuItem> cmd = menu.getCommand();

    bool exit = false;
    std::string message = handleCommand(cmd, exit);

    if (!message.empty()) {
      menu.showMessage(message);
    }

    if (exit)
      isRunning = false;
    else
      std::cout << std::endl;
  }
  SPDLOG_LOGGER_INFO(spdlog::default_logger(), "App exited");
}
