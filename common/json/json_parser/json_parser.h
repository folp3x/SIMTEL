#pragma once

#include <expected>
#include <memory>
#include <nlohmann/json.hpp>

#include "common/json/info/json_field_info/json_field_info.h"

namespace common {
// абстрактный класс для парсинга данных из JSON
template <typename T> class JsonParser {
private:
  std::vector<std::unique_ptr<JsonBaseInfo>> fieldsInfo = {};

protected:
  std::optional<std::string> parseFields(const nlohmann::json &json);

  template <typename F>
  void addParsedField(
      const std::string &name,
      const std::function<void(const F &)> &successCallback,
      const std::function<std::string(const F &)> &checkFn = nullptr);

  template <typename E, size_t S>
  void addParsedArray(
      const std::string &name,
      const std::function<void(const std::array<E, S> &)> &successCallback,
      nlohmann::json::value_t elemType,
      const std::function<std::string(const std::array<E, S> &)> &checkFn =
          nullptr);

  template <typename E>
  void addParsedVector(
      const std::string &name,
      const std::function<void(const std::vector<E> &)> &successCallback,
      nlohmann::json::value_t elemType,
      const std::function<std::string(const std::vector<E> &)> &checkFn =
          nullptr);

  void addParsedObject(const std::string &name,
                       std::vector<std::unique_ptr<JsonBaseInfo>> innerFields);

  virtual void initFields() = 0;

public:
  virtual ~JsonParser() = default;

  virtual std::expected<T, std::string>
  parseJson(const nlohmann::json &json) = 0;

  static std::optional<std::string>
  parseField(const std::unique_ptr<JsonFieldInfo<T>> &fieldInfo,
             const std::string &str);

  std::expected<T, std::string> parse(const std::string &filePath);
};
} // namespace common

#include "json_parser_impl.h"
