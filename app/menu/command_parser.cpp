#include "command_parser.h"

#include <sstream>
#include <variant>

#include "common/constants.h"
#include "core/protocol.h"
#include "utils/str.h"

// парсит агрументы команды 'exit' забирая их из потока
Command CommandParser::parseExitArgs(std::istringstream &stream) const {
  if (hasDataAfterPos(stream.str(), stream.tellg())) {
    // если есть лишние аргументы
    return InvalidCommand{"Error! Redundant argument"};
  }

  return ExitCommand{};
}

// парсит агрументы команды 'active' забирая их из потока
Command CommandParser::parseActiveArgs(std::istringstream &stream) const {
  std::string isActiveStr;
  if (!(stream >> isActiveStr)) {
    return InvalidCommand{"Error! Missing argument"};
  }

  auto parseResult = parseBool(isActiveStr);
  if (parseResult) {
    if (hasDataAfterPos(stream.str(), stream.tellg())) {
      // если есть лишние аргументы
      return InvalidCommand{"Error! Redundant argument"};
    }
    bool isActive = *parseResult;
    return ActiveCommand{isActive};
  }
  return InvalidCommand{"Error! Invalid argument"};
}

// парсит агрументы команды 'move' забирая их из потока
Command CommandParser::parseMoveArgs(std::istringstream &stream) const {
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
      return InvalidCommand{"Error! Not-numeric argument"};
    } catch (const std::out_of_range &) {
      return InvalidCommand{"Error! Argument value out of range"};
    } catch (const std::exception &) {
      return InvalidCommand{"Error! Argument parse error"};
    }
  }

  if (coords.size() == 0) {
    return InvalidCommand{"Error! Missing argument"};
  }

  if (hasDataAfterPos(stream.str(), stream.tellg())) {
    // если есть лишние аргументы
    return InvalidCommand{"Error! Redundant argument"};
  }

  return MoveCommand{coords};
}

// парсит агрументы команды 'protocol' забирая их из потока
Command CommandParser::parseProtocolArgs(std::istringstream &stream) const {
  std::string value;
  if (!(stream >> value)) {
    return InvalidCommand{"Error! Missing argument"};
  }

  if (isCorrectProtocolStr(value)) {
    if (hasDataAfterPos(stream.str(), stream.tellg())) {
      // если есть лишние аргументы
      return InvalidCommand{"Error! Redundant argument"};
    }
    return ProtocolCommand{value};
  }
  return ProtocolCommand{"Error! Invalid argument"};
}

// парсит команду и ее аргументы
Command CommandParser::parseCommand(const std::string &str) const {
  std::istringstream stream(lowercase(str));
  std::string commandName;

  if (!(stream >> commandName)) {
    // если название команды не передано
    return UnknownCommand{};
  }

  const auto &commands = getCommandsInfo();
  auto findResult = commands.find(commandName);
  if (findResult == commands.end()) {
    // если команды нет с списке команд
    return UnknownCommand{};
  }
  CommandInfo info = findResult->second;

  Command cmd = UnknownCommand{};
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
