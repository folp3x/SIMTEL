#include "command_parser.h"

#include <sstream>
#include <variant>

#include "common/constants.h"
#include "core/protocol.h"
#include "utils/str.h"

// парсит агрументы команды 'exit' забирая их из потока
std::unique_ptr<MenuItem>
CommandParser::parseExitArgs(std::istringstream &stream) const {
  if (hasDataAfterPos(stream.str(), stream.tellg())) {
    // если есть лишние аргументы
    return std::make_unique<MenuItemInvalid>("Redundant argument");
  }

  return std::make_unique<MenuItemExit>();
}

// парсит агрументы команды 'active' забирая их из потока
std::unique_ptr<MenuItem>
CommandParser::parseActiveArgs(std::istringstream &stream) const {
  std::string isActiveStr;
  if (!(stream >> isActiveStr)) {
    return std::make_unique<MenuItemInvalid>("Missing argument");
  }

  auto parseResult = parseBool(isActiveStr);
  if (parseResult) {
    if (hasDataAfterPos(stream.str(), stream.tellg())) {
      // если есть лишние аргументы
      return std::make_unique<MenuItemInvalid>("Redundant argument");
    }
    bool isActive = *parseResult;
    return std::make_unique<MenuItemActive>(isActive);
  }
  return std::make_unique<MenuItemInvalid>("Invalid argument");
}

// парсит агрументы команды 'move' забирая их из потока
std::unique_ptr<MenuItem>
CommandParser::parseMoveArgs(std::istringstream &stream) const {
  std::string coordStr;
  std::vector<double> coords = {};
  while (coords.size() < Constants::LOCATION_COORDS_COUNT) {
    if (!(stream >> coordStr)) {
      // если в потоке закончились аргументы
      break;
    }
    try {
      double coord = stod(coordStr);
      coords.push_back(coord);
    } catch (const std::invalid_argument &) {
      return std::make_unique<MenuItemInvalid>("Not-numeric argument");
    } catch (const std::out_of_range &) {
      return std::make_unique<MenuItemInvalid>(
          "Argument value out of range");
    } catch (const std::exception &) {
      return std::make_unique<MenuItemInvalid>("Argument parse error");
    }
  }

  if (coords.size() == 0) {
    return std::make_unique<MenuItemInvalid>("Missing argument");
  }

  if (hasDataAfterPos(stream.str(), stream.tellg())) {
    // если есть лишние аргументы
    return std::make_unique<MenuItemInvalid>("Redundant argument");
  }

  return std::make_unique<MenuItemMove>(coords);
}

// парсит агрументы команды 'protocol' забирая их из потока
std::unique_ptr<MenuItem>
CommandParser::parseProtocolArgs(std::istringstream &stream) const {
  std::string value;
  if (!(stream >> value)) {
    return std::make_unique<MenuItemInvalid>("Missing argument");
  }

  if (isCorrectProtocolStr(value)) {
    if (hasDataAfterPos(stream.str(), stream.tellg())) {
      // если есть лишние аргументы
      return std::make_unique<MenuItemInvalid>("Redundant argument");
    }
    return std::make_unique<MenuItemProtocol>(value);
  }
  return std::make_unique<MenuItemInvalid>("Invalid argument");
}

// парсит команду и ее аргументы
std::unique_ptr<MenuItem>
CommandParser::parseCommand(const std::string &str) const {
  std::istringstream stream(lowercased(str));
  std::string commandName;

  if (!(stream >> commandName)) {
    // если название команды не передано
    return std::make_unique<MenuItemUnknown>();
  }

  const auto &commands = getCommandsInfo();
  auto findResult = commands.find(commandName);
  if (findResult == commands.end()) {
    // если команды нет с списке команд
    return std::make_unique<MenuItemUnknown>();
  }
  CommandInfo info = findResult->second;

  std::unique_ptr<MenuItem> cmd = std::make_unique<MenuItemUnknown>();
  // парсинг аргументов в зависимости от типа команды
  if (commandName == "exit") {
    cmd = parseExitArgs(stream);
  } else if (commandName == "active") {
    cmd = parseActiveArgs(stream);
  } else if (commandName == "move") {
    cmd = parseMoveArgs(stream);
  } else if (commandName == "protocol") {
    cmd = parseProtocolArgs(stream);
  }

  return cmd;
}
