#include "app.h"

#include <iostream>
#include <spdlog/fmt/fmt.h>
#include <stdexcept>
#include <thread>

#include "client/app/menu/menu/menu.h"
#include "client/network/socket/socket.h"
#include "common/app/menu/menu_item/menu_item_exit/menu_item_exit.h"
#include "common/app/menu/menu_item/menu_item_invalid/menu_item_invalid.h"

namespace client {
common::MenuMessage App::formChangeMessage(const std::string &paramName,
                                           const std::string &valueStr,
                                           bool changed) const {
  std::string content = paramName;
  content += changed ? " changed to " : " already set to ";
  content += valueStr;
  return common::MenuMessage{content};
}

std::expected<float, std::string> App::fetchDistance() {
  Socket sock{};
  auto connectError = sock.connectTo(serverAddr);
  if (connectError) {
    return std::unexpected("Error connecting to server: " + *connectError);
  }

  auto sendError = sock.sendLocation(protocol, location);
  if (sendError) {
    return std::unexpected("Error sending location to server: " + *sendError);
  }

  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                     "Location sent to server: {}", location.toStr());

  auto receiveResult = sock.receiveDistance(protocol);
  if (!receiveResult) {
    return std::unexpected("Error receiving distance from server: " +
                           receiveResult.error());
  }

  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                     "Distance received from server: {}",
                     common::toStr(*receiveResult));

  return *receiveResult;
}

void App::handleActiveCommand(const MenuItemActive &cmd) {
  logCommandProcess(cmd.getName(), fmt::format("active={}", cmd.getActive()));

  bool newActive = cmd.getActive();
  bool stateChanged = newActive != inActive;
  if (stateChanged) {
    inActive = newActive;

    if (inActive) {
      auto fetchResult = fetchDistance();
      if (fetchResult) {
        std::lock_guard lock(distanceMtx);
        distance = *fetchResult;
      } else {
        messages.push({fetchResult.error(), common::MenuMessageType::ERR});
      }
    }
  }

  messages.push(
      formChangeMessage("State", appActiveToStr(inActive), stateChanged));
}

void App::handleMoveCommand(const MenuItemMove<> &cmd) {
  auto coords = cmd.getCoords();
  logCommandProcess(
      cmd.getName(),
      fmt::format("coords={}", common::toStr(coords.begin(), coords.end())));

  bool locationChanged = !location.coordsEqual(coords);
  try {
    if (locationChanged) {
      location.move(coords);

      if (inActive) {
        auto fetchResult = fetchDistance();
        if (fetchResult) {
          std::lock_guard lock(distanceMtx);
          distance = *fetchResult;
        } else {
          messages.push({fetchResult.error(), common::MenuMessageType::ERR});
        }
      }
    }

    messages.push(
        formChangeMessage("Location", location.toStr(), locationChanged));
  } catch (const std::invalid_argument &e) {
    messages.push({"Location coords count is invalid"});
  }
}

void App::handleProtocolCommand(const MenuItemProtocol &cmd) {
  std::string protocolStr = cmd.getProtocol();
  logCommandProcess(cmd.getName(), fmt::format("protocol={}", protocolStr));

  auto protocolParseResult = common::protocolFromStr(protocolStr);
  if (protocolParseResult) {
    common::Protocol newProtocol = *protocolParseResult;

    bool protocolChanged = newProtocol != protocol;
    if (protocolChanged) {
      protocol = newProtocol;
    }

    messages.push({formChangeMessage("Protocol", protocolToStr(protocol),
                                     protocolChanged)});
    return;
  }

  messages.push({"Invalid protocol"});
}

void App::handleCommand(const std::unique_ptr<common::MenuItem> &cmd,
                        bool &exit) {
  std::string cmdNameUpper = common::uppercased(cmd->getName());
  bool isCorrectCommand = true;

  // выполнение команды в засимости от ее типа
  if (auto *invalidCmd = dynamic_cast<common::MenuItemInvalid *>(cmd.get())) {
    SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                       "Received invalid command: {}", invalidCmd->getError());
    messages.push(
        {"Error! " + invalidCmd->getError(), common::MenuMessageType::ERR});
    isCorrectCommand = false;
  } else if (dynamic_cast<common::MenuItemExit *>(cmd.get())) {
    logCommandProcess(cmdNameUpper);
    messages.push({"Exiting app..."});
    exit = true;
  } else if (auto *activeCmd = dynamic_cast<MenuItemActive *>(cmd.get())) {
    handleActiveCommand(*activeCmd);
  } else if (auto *moveCmd = dynamic_cast<MenuItemMove<> *>(cmd.get())) {
    handleMoveCommand(*moveCmd);
  } else if (auto *protocolCmd = dynamic_cast<MenuItemProtocol *>(cmd.get())) {
    handleProtocolCommand(*protocolCmd);
  }

  if (isCorrectCommand) {
    SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                       "Finished command {}", cmdNameUpper);
  }
}

void App::updateDistance(int updateFreqSec) {
  while (true) {
    if (inActive) {
      auto fetchResult = fetchDistance();
      if (fetchResult) {
        std::lock_guard lock(distanceMtx);
        distance = *fetchResult;
        continue;
      }

      SPDLOG_LOGGER_WARN(common::Logger::instance().getInner(),
                         "Error updating distance: {}", fetchResult.error());
    }
    std::this_thread::sleep_for(std::chrono::seconds(updateFreqSec));
  }
}

App::App(const common::Location<> &location_, const common::imsi_t &imsi_,
         const common::imei_t &imei_, const common::NetworkAddress &serverAddr_,
         const std::vector<AddressBookRecord> &addressBook_)
    : common::App<Config>(location_), imsi(imsi_), imei(imei_),
      serverAddr(serverAddr_), addressBook(addressBook_) {}

void App::run() {
  Menu menu;
  messages = {};
  isRunning = true;

  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(), "App started");
  while (isRunning) {
    menu.showMenuHeaderLine();
    menu.showStatus(inActive, imsi, location, protocol);
    menu.showMenuHeaderLine();
    menu.showDistance(serverAddr, distance);
    menu.showMenuHeaderLine();
    menu.showAddressBook(addressBook);
    menu.showMenuHeaderLine();
    menu.showCommandsInfo(getCommandsInfo());

    std::string extraMsgContent = "";
    auto cmd = menu.getCommand(extraMsgContent);

    if (!extraMsgContent.empty()) {
      menu.showMessage({extraMsgContent});
    }

    bool exit = false;
    handleCommand(cmd, exit);
    menu.showMessages(messages);

    if (exit) {
      isRunning = false;
    } else {
      std::cout << std::endl;
    }
  }
  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(), "App exited");
}
} // namespace client
