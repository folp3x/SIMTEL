#pragma once

namespace client {
template <typename T>
  requires std::is_arithmetic_v<T>
std::unique_ptr<common::MenuItem>
CommandParser::parseMoveArgs(const std::string &initialStr,
                             const std::vector<std::string> &args,
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

    auto coord = common::utils::fromString<T>(args[i]);
    if (coord) {
      coords.push_back(*coord);
    } else {
      return std::make_unique<common::MenuItemInvalid>(
          "Argument parse error: " + coord.error());
    }
  }

  if (args.size() > requiredArgsCount) {
    extraMsg = "Extra arguments ignored. Only " +
               common::utils::toStr(coords.begin(), coords.end()) + " used";
  }

  return std::make_unique<MenuItemMove<T>>(coords);
}

template <std::derived_from<common::MenuItem> T>
std::unique_ptr<common::MenuItem>
CommandParser::parseWithoutArgs(const std::string &initialStr,
                                const std::vector<std::string> &args,
                                std::string &extraMsg) {
  if (!args.empty()) {
    extraMsg = "Extra arguments ignored";
  }
  return std::make_unique<T>();
}
} // namespace client
