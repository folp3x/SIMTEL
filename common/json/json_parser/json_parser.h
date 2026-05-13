#pragma once

// вывод пути к месту где произошла ошибка при разборе json
#define JSON_DIAGNOSTICS 1

#include <expected>
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>

#include "common/json/info/json_array_info/json_array_info.h"
#include "common/json/info/json_base_info/json_base_info.h"
#include "common/json/info/json_field_info/json_field_info.h"

namespace common {
// абстрактный класс для парсинга данных из JSON
template <typename T> class JsonParser {
private:
  std::vector<std::unique_ptr<JsonBaseInfo>> fieldsInfo = {};

protected:
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

  virtual void initFields() = 0;

  virtual std::expected<T, std::string>
  parseJson(const nlohmann::json &json) = 0;

public:
  virtual ~JsonParser() = default;

  virtual std::expected<T, std::string> parse(const std::string &filePath) {
    std::ifstream file(filePath);
    if (!file.is_open())
      return std::unexpected("Can't open file '" + filePath + "'");

    nlohmann::json json;

    try {
      file >> json;

      if (file.fail())
        return std::unexpected("File error");

      return parseJson(json);
    } catch (const nlohmann::json::parse_error &e) {
      return std::unexpected("JSON parse error: " + std::string(e.what()));
    } catch (const nlohmann::json::out_of_range &e) {
      return std::unexpected("JSON out of range error: " +
                             std::string(e.what()));
    }
  }
};
} // namespace common
