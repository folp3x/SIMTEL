#include "locale.h"

namespace client {
static const std::unordered_map<std::string, std::string> locales = {
    {"ru", "ru_RU.UTF-8"}, {"en", "en_US.UTF-8"}};

std::string isSupportedLocale(const std::string &alias) {
  return (locales.find(alias) != locales.end()) ? "" : "Locale not supported";
}

std::optional<std::string> localeNameFromAlias(const std::string &alias) {
  auto it = locales.find(alias);
  if (it == locales.end()) {
    return std::nullopt;
  }

  return it->second;
}
} // namespace client
