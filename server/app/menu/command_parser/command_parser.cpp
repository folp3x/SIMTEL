#include "command_parser.h"

#include "common/app/menu/menu_item/menu_item_invalid/menu_item_invalid.h"
#include "common/constants/constants.h"
#include "common/utils/str/str.h"
#include "server/app/menu/menu_item/menu_item_dist/menu_item_dist.h"

namespace server {
common::ArgsParsersMap CommandParser::getArgsParsers() const {
  return argsParsers;
}

std::unique_ptr<common::MenuItem>
CommandParser::parseDistArgs(const std::vector<std::string> &args,
                             std::string &extraMsg) {
  const int requiredArgsCount = MenuItemDist::getArgsCount();

  if (args.size() < requiredArgsCount)
    return std::make_unique<common::MenuItemInvalid>("Missing arguments");

  std::vector<float> coords;
  coords.reserve(requiredArgsCount);

  for (int i = 0; i < args.size(); ++i) {
    if (i >= requiredArgsCount)
      break;

    auto coordsParseResult = common::fromString<float>(args[i]);
    if (coordsParseResult) {
      coords.push_back(*coordsParseResult);
    } else {
      return std::make_unique<common::MenuItemInvalid>(
          "Argument parse error: " + coordsParseResult.error());
    }
  }

  if (args.size() > requiredArgsCount) {
    extraMsg = "Extra arguments ignored. Only " +
               common::toStr(coords.begin(), coords.end()) + " used";
  }

  return std::make_unique<MenuItemDist>(coords);
}
} // namespace server
