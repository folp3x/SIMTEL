#include "app.h"

#include <iostream>
#include <stdexcept>
#include <variant>

#include "menu/menu.h"

// выполняет команду 'active', возвращает сообщение
std::string App::handleActiveCommand(const MenuItemActive &cmd) {
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
  if (!location.coordsEqual(cmd.getCoords())) {
    location.move(cmd.getCoords());
    return "Position changed to " + location.toStr();
  }
  return "Position already set to " + location.toStr();
}

// выполняет команду 'protocol', возвращает сообщение
std::string App::handleProtocolCommand(const MenuItemProtocol &cmd) {
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

    std::unique_ptr<MenuItem> cmd = menu.getCommand();
    // выполнение команды в засимости от ее типа
    if (dynamic_cast<MenuItemUnknown *>(cmd.get())) {
      message = "Unknown command";
    } else if (auto *invalidCmd = dynamic_cast<MenuItemInvalid *>(cmd.get())) {
      message = invalidCmd->getError();
    } else if (dynamic_cast<MenuItemExit *>(cmd.get())) {
      message = "Exiting app...";
      isRunning = false;
    } else if (auto *activeCmd = dynamic_cast<MenuItemActive *>(cmd.get())) {
      message = handleActiveCommand(*activeCmd);
    } else if (auto *moveCmd = dynamic_cast<MenuItemMove *>(cmd.get())) {
      message = handleMoveCommand(*moveCmd);
    } else if (auto *protocolCmd =
                   dynamic_cast<MenuItemProtocol *>(cmd.get())) {
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
