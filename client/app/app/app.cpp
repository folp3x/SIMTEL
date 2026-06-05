#include "app.h"

#include <iostream>
#include <spdlog/fmt/fmt.h>
#include <stdexcept>
#include <thread>

#include "client/app/menu/menu_item/menu_item_dialog/menu_item_dialog.h"
#include "client/app/menu/menu_item/menu_item_sms/menu_item_sms.h"
#include "client/network/socket/socket.h"
#include "common/app/menu/menu_item/menu_item_exit/menu_item_exit.h"
#include "common/app/menu/menu_item/menu_item_invalid/menu_item_invalid.h"
#include "common/utils/str/str.h"

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

void App::handleSmsCommand(const MenuItemSMS &cmd) {
  common::msisdn_t targetMsisdn = "";
  if (cmd.getSpeedDialNum() != constants::EMPTY_SPEED_DIAL_NUM) {
    auto findResult = findBySpeedDialNum(cmd.getSpeedDialNum());
    if (!findResult) {
      messages.push({"Unknown speed dial num"});
    } else {
      targetMsisdn = std::move(*findResult);
    }
  }

  std::string smsContent = "";
  if (!cmd.getContent().empty()) {
    smsContent = cmd.getContent();
  } else {
    smsContent = menu.getMessageContent();
    if (smsContent.empty()) {
      messages.push(
          {"SMS content cant be empty", common::MenuMessageType::ERR});
    } else {
      // удаление '\n'
      smsContent.pop_back();
    }
  }

  messages.push({"SMS: " + targetMsisdn + ", " + smsContent});
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
  } else if (auto *smsCmd = dynamic_cast<MenuItemSMS *>(cmd.get())) {
    handleSmsCommand(*smsCmd);
  } else if (auto *sentCmd = dynamic_cast<MenuItemSent *>(cmd.get())) {
    messages.push({"SENT"});
  } else if (auto *receivedCmd = dynamic_cast<MenuItemReceived *>(cmd.get())) {
    messages.push({"RECEIVED"});
  } else if (auto *dialogCmd = dynamic_cast<MenuItemDialog *>(cmd.get())) {
    messages.push({"DIALOG"});
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

std::optional<common::msisdn_t> App::findBySpeedDialNum(char num) {
  auto it = addressBook.find(num);
  if (it == addressBook.end()) {
    return std::nullopt;
  }
  return it->second;
}

App::App(const common::Location<> &location_, const common::imsi_t &imsi_,
         const common::imei_t &imei_, const common::NetworkAddress &serverAddr_,
         const std::map<char, common::msisdn_t> &addressBook_)
    : location(location_), imsi(imsi_), imei(imei_), serverAddr(serverAddr_),
      addressBook(addressBook_) {}

void App::run() {
  messages = {};
  isRunning = true;

  SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(), "App started");
  while (isRunning) {
    menu.showMenuHeaderLine();
    menu.showStatus(inActive, imsi, protocol);
    menu.showMenuHeaderLine();
    menu.showSignalInfo(location, distance);
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

void App::logCommandProcess(std::string_view commandName,
                            std::string_view argsStr) const {
  std::string nameUpper = common::uppercased(commandName);
  if (!argsStr.empty()) {
    SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                       "Processing command {} with args: {}", nameUpper,
                       argsStr);
  } else {
    SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                       "Processing command {}", nameUpper);
  }
}
} // namespace client
