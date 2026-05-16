#pragma once

#include "common/logging/logger/logger.h"
#include "common/utils/str/str.h"

namespace common {
template <std::derived_from<Config> T>
void App<T>::logConstructor(const std::string &constructorType,
                            const Location<> &location) const {
  SPDLOG_LOGGER_DEBUG(common::Logger::instance().getInner(),
                      "common::App {} constructor called: location={}",
                      constructorType, location.toStr());
}

template <std::derived_from<Config> T>
void App<T>::logCommandProcess(std::string_view commandName,
                               std::string_view argsStr) const {
  std::string nameUpper = uppercased(commandName);
  if (!argsStr.empty()) {
    SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                       "Processing command {} with args: {}", nameUpper,
                       argsStr);
  } else {
    SPDLOG_LOGGER_INFO(common::Logger::instance().getInner(),
                       "Processing command {}",
                       nameUpper);
  }
}

template <std::derived_from<Config> T>
App<T>::App(const Location<> &location_) : location(location_) {}

template <std::derived_from<Config> T>
App<T>::App(const App &other) : location(other.location) {
  logConstructor("COPY", location);
}

template <std::derived_from<Config> T>
App<T>::App(App &&other) noexcept : location(std::move(other.location)) {
  logConstructor("MOVE", location);
}
} // namespace common
