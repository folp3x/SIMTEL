#pragma once

#include "client/app/menu/menu_item/menu_item_move/menu_item_move.h"
#include "common/app/menu/menu_item/menu_item_invalid/menu_item_invalid.h"

namespace client {
template <typename T>
std::unique_ptr<common::MenuItem>
CommandParser::parseMoveArgs(const std::vector<std::string> &args,
                             std::string &extraMsg) {
  const size_t requiredArgsCount = MenuItemMove<T>::getArgsCount();

  if (args.empty()) {
    return std::make_unique<common::MenuItemInvalid>("Missing argument");
  }

  std::vector<T> coords;
  coords.reserve(requiredArgsCount);

  for (size_t i = 0; i < args.size(); ++i) {
    if (i >= requiredArgsCount) {
      break;
    }

    auto coordsParseResult = common::fromString<T>(args[i]);
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

  return std::make_unique<MenuItemMove<T>>(coords);
}
} // namespace client
