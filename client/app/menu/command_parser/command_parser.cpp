#include "command_parser.h"

#include "client/app/menu/menu_item/menu_item_active/menu_item_active.h"
#include "client/app/menu/menu_item/menu_item_move/menu_item_move.h"
#include "client/app/menu/menu_item/menu_item_protocol/menu_item_protocol.h"
#include "common/app/menu/menu_item/menu_item_invalid/menu_item_invalid.h"
#include "common/constants/constants.h"
#include "common/network/protocol/protocol.h"
#include "common/utils/str/str.h"

namespace client {
common::ArgsParsersMap CommandParser::getArgsParsers() const {
  return argsParsers;
}

std::unique_ptr<common::MenuItem>
CommandParser::parseActiveArgs(const std::vector<std::string> &args,
                               std::string &extraMsg) {

  if (args.empty())
    return std::make_unique<common::MenuItemInvalid>("Missing argument");

  std::string isActiveStr = args[0];

  auto parseResult = common::parseBool(isActiveStr);
  if (parseResult) {
    if (args.size() > MenuItemActive::getArgsCount())
      extraMsg = "Extra arguments ignored";

    bool isActive = *parseResult;
    return std::make_unique<MenuItemActive>(isActive);
  }
  return std::make_unique<common::MenuItemInvalid>("Invalid argument");
}

std::unique_ptr<common::MenuItem>
CommandParser::parseMoveArgs(const std::vector<std::string> &args,
                             std::string &extraMsg) {
  const int requiredArgsCount = MenuItemMove::getArgsCount();

  if (args.empty())
    return std::make_unique<common::MenuItemInvalid>("Missing argument");

  std::vector<float> coords;
  coords.reserve(requiredArgsCount);

  for (int i = 0; i < args.size(); ++i) {
    if (i >= requiredArgsCount)
      break;

    auto coordsParseResult = common::fromString<float>(args[i]);
    if (coordsParseResult)
      coords.push_back(*coordsParseResult);
    else
      return std::make_unique<common::MenuItemInvalid>(
          "Argument parse error: " + coordsParseResult.error());
  }

  if (args.size() > requiredArgsCount) {
    extraMsg = "Extra arguments ignored. Only " +
               common::toStr(coords.begin(), coords.end()) + " used";
  }

  return std::make_unique<MenuItemMove>(coords);
}

std::unique_ptr<common::MenuItem>
CommandParser::parseProtocolArgs(const std::vector<std::string> &args,
                                 std::string &extraMsg) {
  if (args.empty())
    return std::make_unique<common::MenuItemInvalid>("Missing argument");

  std::string value = args[0];

  auto nameFindResult = common::protocolNameFromAlias(value);
  std::string name;
  if (nameFindResult)
    name = *nameFindResult;
  else
    name = value;

  if (common::isCorrectProtocolStr(name)) {
    if (args.size() > MenuItemProtocol::getArgsCount()) {
      extraMsg = "Extra arguments ignored";
    }
    return std::make_unique<MenuItemProtocol>(name);
  }
  return std::make_unique<common::MenuItemInvalid>("Invalid argument");
}
} // namespace client
