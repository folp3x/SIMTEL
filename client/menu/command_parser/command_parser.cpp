#include "command_parser.h"

#include <sstream>
#include <variant>

#include "common/constants/constants.h"
#include "common/network/protocol/protocol.h"
#include "common/utils/str/str.h"

namespace client {
std::unique_ptr<common::MenuItem>
CommandParser::parseExitArgs(std::istringstream &stream,
                             std::string &extraMsg) const {
  if (common::hasDataAfterPos(stream.str(), stream.tellg())) {
    // если есть лишние аргументы
    extraMsg = "Redundant arguments ignored";
  }

  return std::make_unique<MenuItemExit>();
}

std::unique_ptr<common::MenuItem>
CommandParser::parseActiveArgs(std::istringstream &stream,
                               std::string &extraMsg) const {
  std::string isActiveStr;
  if (!(stream >> isActiveStr))
    return std::make_unique<MenuItemInvalid>("Missing argument");

  auto parseResult = common::parseBool(isActiveStr);
  if (parseResult) {
    if (common::hasDataAfterPos(stream.str(), stream.tellg())) {
      // если есть лишние аргументы
      extraMsg = "Redundant arguments ignored";
    }
    bool isActive = *parseResult;
    return std::make_unique<MenuItemActive>(isActive);
  }
  return std::make_unique<MenuItemInvalid>("Invalid argument");
}

std::unique_ptr<common::MenuItem>
CommandParser::parseMoveArgs(std::istringstream &stream,
                             std::string &extraMsg) const {
  std::string coordStr;
  std::vector<float> coords;
  coords.reserve(common::constants::LOCATION_COORDS_COUNT);
  while (coords.size() < common::constants::LOCATION_COORDS_COUNT) {
    if (!(stream >> coordStr)) {
      // если в потоке закончились аргументы
      break;
    }
    try {
      float coord = stod(coordStr);
      coords.push_back(coord);
    } catch (const std::invalid_argument &) {
      return std::make_unique<MenuItemInvalid>("Not-numeric argument");
    } catch (const std::out_of_range &) {
      return std::make_unique<MenuItemInvalid>("Argument value out of range");
    }
  }

  if (coords.size() == 0)
    return std::make_unique<MenuItemInvalid>("Missing argument");

  if (common::hasDataAfterPos(stream.str(), stream.tellg())) {
    // если есть лишние аргументы
    extraMsg = "Redundant arguments ignored";
  }

  return std::make_unique<MenuItemMove>(coords);
}

std::unique_ptr<common::MenuItem>
CommandParser::parseProtocolArgs(std::istringstream &stream,
                                 std::string &extraMsg) const {
  std::string value;
  if (!(stream >> value))
    return std::make_unique<MenuItemInvalid>("Missing argument");

  if (common::isCorrectProtocolStr(value)) {
    if (common::hasDataAfterPos(stream.str(), stream.tellg())) {
      // если есть лишние аргументы
      extraMsg = "Redundant arguments ignored";
    }
    return std::make_unique<MenuItemProtocol>(value);
  }
  return std::make_unique<MenuItemInvalid>("Invalid argument");
}

// парсит команду и ее аргументы
std::unique_ptr<common::MenuItem>
CommandParser::parseCommand(const std::string &str,
                            std::string &extraMsg) const {
  std::istringstream stream(common::lowercased(common::ltrimmed(str)));
  std::string commandName;

  if (!(stream >> commandName))
    return std::make_unique<MenuItemUnknown>();

  const auto &commands = getCommandsInfo();
  auto infoIt = commands.find(commandName);
  if (infoIt == commands.end()) {
    // если команды нет с списке команд
    return std::make_unique<MenuItemUnknown>();
  }
  common::CommandInfo info = infoIt->second;

  auto parserIt = argsParsers.find(commandName);
  if (parserIt == argsParsers.end()) {
    // если для команды нет обработчика
    return std::make_unique<MenuItemUnknown>();
  }

  auto cmd = parserIt->second(stream, extraMsg);
  return cmd;
}
} // namespace client
