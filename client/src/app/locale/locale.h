#pragma once

#include <optional>
#include <string>
#include <unordered_map>

namespace client {
std::string isSupportedLocale(const std::string &alias);
std::optional<std::string> localeNameFromAlias(const std::string &alias);
} // namespace client
