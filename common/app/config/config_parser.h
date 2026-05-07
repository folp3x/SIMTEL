#pragma once

#include <concepts>
#include <expected>
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>

#include "common/constants.h"
#include "common/validator/validator.h"
#include "config.h"
#include "config_array_info.h"
#include "config_field_base.h"
#include "config_field_info.h"

namespace common {
// абстрактный класс для парсинга конфигурации из JSON
template <std::derived_from<Config> T> class ConfigParser {
private:
  std::vector<std::unique_ptr<ConfigFieldBase>> fieldsInfo = {};

  bool parseFields(const nlohmann::json &json, std::string &msg) const {
    for (const auto &info : fieldsInfo) {
      if (!json.contains(info->getName())) {
        msg = info->getName(true) + " is required";
        return false;
      }
      bool result = info->parse(json, msg);
      if (!result)
        return false;
    }
    return true;
  }

protected:
  T config{};
  std::array<double, constants::LOCATION_COORDS_COUNT> tempLoc = {};
  ConfigParser() = default;

  // добавление в список полей простого JSON-поля
  template <typename F>
  void addParsedField(
      const std::string &name, F *field, nlohmann::detail::value_t type,
      const std::function<std::string(const F &)> &checkFn = nullptr) {
    auto info =
        std::make_unique<ConfigFieldInfo<F>>(name, field, type, checkFn);
    fieldsInfo.push_back(std::move(info));
  }

  // добавление в список полей JSON-поля с типом массив
  template <typename E, size_t S>
  void addParsedArray(const std::string &name, std::array<E, S> *field,
                      nlohmann::detail::value_t elemType,
                      const std::function<std::string(const std::array<E, S> &)>
                          &checkFn = nullptr) {
    auto info =
        std::make_unique<ConfigArrayInfo<E, S>>(name, field, elemType, checkFn);
    fieldsInfo.push_back(std::move(info));
  }

  virtual void initIpField() {
    addParsedField<std::string>("ip", &config.ip,
                                nlohmann::detail::value_t::string,
                                Validator::isCorrectIpStr);
  }

  virtual void initPortField() {
    addParsedField<int>("port", &config.port,
                        nlohmann::detail::value_t::number_unsigned,
                        Validator::isCorrectPort);
  }

  virtual void initLocField() {
    addParsedArray<double, constants::LOCATION_COORDS_COUNT>(
        "loc", &tempLoc, nlohmann::detail::value_t::number_float);
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

      std::string msg = "";
      bool parsed = parseFields(json, msg);
      if (!parsed) {
        return std::unexpected(msg);
      }

      T configCopy = config;
      configCopy.setLoc(tempLoc);
      return configCopy;
    } catch (const nlohmann::json::parse_error &e) {
      return std::unexpected("JSON parse error: " + std::string(e.what()));
    } catch (const nlohmann::json::type_error &e) {
      return std::unexpected("JSON type error: " + std::string(e.what()));
    } catch (const nlohmann::json::out_of_range &e) {
      return std::unexpected("JSON out of range error: " +
                             std::string(e.what()));
    } catch (const std::exception &e) {
      return std::unexpected("JSON error: " + std::string(e.what()));
    }
  }
};
} // namespace common
