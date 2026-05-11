#pragma once

#include "common/json/json_parser/json_parser.h"

#include <concepts>

#include "common/app/config/config/config.h"
#include "common/constants/constants.h"
#include "common/validator/validator.h"

namespace common {
// базовый класс для парсинга конфигурации из JSON
template <std::derived_from<Config> T>
class ConfigParser : public JsonParser<T> {
protected:
  T config{};

  ConfigParser() = default;

  void initIpField() {
    this->template addParsedField<std::string>(
        "ip", [this](const std::string &ip) { config.setIP(ip); },
        nlohmann::json::value_t::string, Validator::isCorrectIpStr);
  }

  void initPortField() {
    this->template addParsedField<int>(
        "port", [this](int port) { config.setPort(port); },
        nlohmann::json::value_t::number_unsigned, Validator::isCorrectPort);
  }

  void initLocField() {
    this->template addParsedArray<float, constants::LOCATION_COORDS_COUNT>(
        "loc",
        [this](const common::coords_t<float> &loc) { config.setLoc(loc); },
        nlohmann::json::value_t::number_float);
  }

  virtual void initFields() {
    initIpField();
    initPortField();
    initLocField();
  }

public:
  virtual ~ConfigParser() = default;

  static std::unique_ptr<ConfigParser> create() {
    auto parser = std::make_unique<ConfigParser>();
    parser->initFields();
    return parser;
  }

  virtual std::expected<T, std::string>
  parseJson(const nlohmann::json &json) override {
    config = T{};

    auto error = this->parseFields(json);
    if (error)
      return std::unexpected(*error);

    return config;
  }
};
} // namespace common
