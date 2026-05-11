#include "command_parser.h"

#include <variant>

#include "common/constants/constants.h"
#include "common/network/protocol/protocol.h"
#include "common/utils/str/str.h"

namespace client {
std::unique_ptr<common::MenuItem>
CommandParser::parseExitArgs(const std::vector<std::string> &args,
                             std::string &extraMsg) {
  if (args.size() > MenuItemExit::getArgsCount())
    extraMsg = "Extra arguments ignored";

  return std::make_unique<MenuItemExit>();
}

std::unique_ptr<common::MenuItem>
CommandParser::parseActiveArgs(const std::vector<std::string> &args,
                               std::string &extraMsg) {

  if (args.empty())
    return std::make_unique<MenuItemInvalid>("Missing argument");

  std::string isActiveStr = args[0];

  auto parseResult = common::parseBool(isActiveStr);
  if (parseResult) {
    if (args.size() > MenuItemExit::getArgsCount())
      extraMsg = "Extra arguments ignored";

    bool isActive = *parseResult;
    return std::make_unique<MenuItemActive>(isActive);
  }
  return std::make_unique<MenuItemInvalid>("Invalid argument");
}

std::unique_ptr<common::MenuItem>
CommandParser::parseMoveArgs(const std::vector<std::string> &args,
                             std::string &extraMsg) {
  if (args.empty())
    return std::make_unique<MenuItemInvalid>("Missing argument");

  std::vector<float> coords;
  coords.reserve(args.size());

  for (int i = 0; i < args.size(); ++i) {
    if (i > common::constants::LOCATION_COORDS_COUNT - 1)
      break;

    auto coordsParseResult = common::fromString<float>(args[i]);
    if (coordsParseResult)
      coords.push_back(*coordsParseResult);
    else
      return std::make_unique<MenuItemInvalid>("Argument parse error: " +
                                               coordsParseResult.error());
  }

  if (args.size() > MenuItemMove::getArgsCount())
    extraMsg = "Extra arguments ignored";

  return std::make_unique<MenuItemMove>(coords);
}

std::unique_ptr<common::MenuItem>
CommandParser::parseProtocolArgs(const std::vector<std::string> &args,
                                 std::string &extraMsg) {
  if (args.empty())
    return std::make_unique<MenuItemInvalid>("Missing argument");

  std::string value = args[0];

  if (common::isCorrectProtocolStr(value)) {
    if (args.size() > MenuItemProtocol::getArgsCount()) {
      extraMsg = "Extra arguments ignored";
    }
    return std::make_unique<MenuItemProtocol>(value);
  }
  return std::make_unique<MenuItemInvalid>("Invalid argument");
}

// парсит команду и ее аргументы
std::unique_ptr<common::MenuItem>
CommandParser::parseCommand(const std::string &str,
                            std::string &extraMsg) const {
  std::vector<std::string> tokens =
      common::split(common::lowercased(common::ltrimmed(str)));

  if (tokens.empty())
    return std::make_unique<MenuItemUnknown>();

  std::string commandName = tokens[0];

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

  tokens.erase(tokens.begin());

  auto cmd = parserIt->second(tokens, extraMsg);
  return cmd;
}
} // namespace client
