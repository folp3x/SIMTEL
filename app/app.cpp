#include "app.h"

#include <iostream>
#include <stdexcept>
#include <variant>

#include "menu/menu.h"

// выполняет команду 'active', возвращает сообщение
std::string App::handleActiveCommand(const ActiveCommand &cmd) {
  AppState newState = cmd.isActive ? AppState::ACTIVE : AppState::INACTIVE;

  std::string stateStr = appStateToStr(newState);
  if (newState != state) {
    state = newState;
    return "Status changed to " + stateStr;
  }
  return "Status already set to " + stateStr;
}

// выполняет команду 'move', возвращает сообщение
std::string App::handleMoveCommand(const MoveCommand &cmd) {
  if (!location.coordsEqual(cmd.coords)) {
    location.move(cmd.coords);
    return "Position changed to " + location.toStr();
  }
  return "Position already set to " + location.toStr();
}

// выполняет команду 'protocol', возвращает сообщение
std::string App::handleProtocolCommand(const ProtocolCommand &cmd) {
  auto protocolParseResult = protocolFromStr(cmd.value);
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

App::App(const Config &config)
    : ip(config.getIP()), port(config.getPort()), imei(config.getImei()),
      imsi(config.getImsi()) {
  location.move(config.getLoc());
}

// получает команды от пользователя через меню и выполняет их
void App::run() {
  Menu menu;
  // сообщение с результатами выполнения команды
  std::string message = "";

  bool isRunning = true;

  while (isRunning) {
    menu.showStatus(state, imsi, location, protocol);
    menu.showCommandsInfo();

    Command cmd = menu.getCommand();
    // выполнение команды в засимости от ее типа
    if (std::get_if<UnknownCommand>(&cmd)) {
      message = "Unknown command";
    } else if (auto *invalidCmd = std::get_if<InvalidCommand>(&cmd)) {
      message = invalidCmd->error;
    } else if (auto *exit = std::get_if<ExitCommand>(&cmd)) {
      message = "Exiting app...";
      isRunning = false;
    } else if (auto *activeCmd = std::get_if<ActiveCommand>(&cmd)) {
      message = handleActiveCommand(*activeCmd);
    } else if (auto *moveCmd = std::get_if<MoveCommand>(&cmd)) {
      message = handleMoveCommand(*moveCmd);
    } else if (auto *protocolCmd = std::get_if<ProtocolCommand>(&cmd)) {
      message = handleProtocolCommand(*protocolCmd);
    } else {
      message = "";
    }

    if (!message.empty()) {
      menu.showMessage(message);
    }
    std::cout << std::endl;
  }
}
