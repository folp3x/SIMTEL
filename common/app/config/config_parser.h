#pragma once

// вывод пути к месту где произошла ошибка при разборе json
#define JSON_DIAGNOSTICS 1

#include <concepts>
#include <expected>
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>

#include "common/constants.h"
#include "common/json/json_array_info.h"
#include "common/json/json_base_info.h"
#include "common/json/json_field_info.h"
#include "common/validator/validator.h"
#include "config.h"

namespace common {
// абстрактный класс для парсинга конфигурации из JSON
template <std::derived_from<Config> T> class ConfigParser {
private:
  std::vector<std::unique_ptr<JsonBaseInfo>> fieldsInfo = {};

  std::optional<std::string> parseFields(const nlohmann::json &json) {
    for (const auto &info : fieldsInfo) {
      if (!json.contains(info->getName()))
        return info->getName(true) + " is required";

      auto error = info->parse(json);
      if (error)
        return *error;
    }
    return std::nullopt;
  }

protected:
  T config{};

  ConfigParser() = default;

  // добавление в список полей простого JSON-поля
  template <typename F>
  void addParsedField(
      const std::string &name,
      const std::function<void(const F &)> successCallback,
      nlohmann::json::value_t type,
      const std::function<std::string(const F &)> &checkFn = nullptr) {
    auto info = std::make_unique<JsonFieldInfo<F>>(name, successCallback, type,
                                                   checkFn);
    fieldsInfo.push_back(std::move(info));
  }

  // добавление в список полей JSON-поля с типом массив
  template <typename E, size_t S>
  void addParsedArray(
      const std::string &name,
      const std::function<void(const std::array<E, S> &)> successCallback,
      nlohmann::json::value_t elemType,
      const std::function<std::string(const std::array<E, S> &)> &checkFn =
          nullptr) {
    auto info = std::make_unique<JsonArrayInfo<E, S>>(name, successCallback,
                                                      elemType, checkFn);
    fieldsInfo.push_back(std::move(info));
  }

  void initIpField() {
    addParsedField<std::string>(
        "ip", [this](const std::string &ip) { config.setIP(ip); },
        nlohmann::json::value_t::string, Validator::isCorrectIpStr);
  }

  void initPortField() {
    addParsedField<int>(
        "port", [this](int port) { config.setPort(port); },
        nlohmann::json::value_t::number_unsigned, Validator::isCorrectPort);
  }

  void initLocField() {
    addParsedArray<float, constants::LOCATION_COORDS_COUNT>(
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

  std::expected<T, std::string> parse(const std::string &filePath) {
    std::ifstream file(filePath);
    if (!file.is_open())
      return std::unexpected("Can't open file '" + filePath + "'");

    nlohmann::json json;

    try {
      file >> json;

      if (file.fail())
        return std::unexpected("File error");

      // очистка конфига
      config = T{};

      auto error = parseFields(json);
      if (error)
        return std::unexpected(*error);
      return config;
    } catch (const nlohmann::json::parse_error &e) {
      return std::unexpected("JSON parse error: " + std::string(e.what()));
    } catch (const nlohmann::json::out_of_range &e) {
      return std::unexpected("JSON out of range error: " +
                             std::string(e.what()));
    }
  }
};
} // namespace common
