#pragma once

#include "common/validator/validator.h"

namespace common {
template <std::derived_from<Config> T> void ConfigParser<T>::initPortField() {
  this->template addParsedField<int>(
      "port", [this](int port) { config.setPort(port); },
      nlohmann::json::value_t::number_unsigned, Validator::isCorrectPort);
}

template <std::derived_from<Config> T> void ConfigParser<T>::initLocField() {
  this->template addParsedArray<float, constants::LOCATION_COORDS_COUNT>(
      "loc", [this](const common::coords_t<> &loc) { config.setLoc(loc); },
      nlohmann::json::value_t::number_float);
}

template <std::derived_from<Config> T> void ConfigParser<T>::initFields() {
  initPortField();
  initLocField();
}

template <std::derived_from<Config> T>
std::unique_ptr<ConfigParser<T>> ConfigParser<T>::create() {
  auto parser = std::unique_ptr<ConfigParser<T>>(new ConfigParser<T>());
  parser->initFields();
  return parser;
}

template <std::derived_from<Config> T>
std::expected<T, std::string>
ConfigParser<T>::parseJson(const nlohmann::json &json) {
  // очистка конфига
  config = T{};

  auto error = this->parseFields(json);
  if (error) {
    return std::unexpected(*error);
  }

  return config;
}
} // namespace common
