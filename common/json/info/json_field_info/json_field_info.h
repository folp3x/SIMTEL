#pragma once

#include <functional>
#include <nlohmann/json.hpp>
#include <optional>

#include "common/json/info/json_base_info/json_base_info.h"
#include "common/json/json_type/json_type.h"

namespace common {
// класс с информацией для парсинга простого JSON-поля
template <typename T> class JsonFieldInfo : public JsonBaseInfo {
private:
  nlohmann::json::value_t type;
  std::function<std::string(const T &)> checkFn;

protected:
  std::string name = "";
  T *field;
  std::function<void(const T &)> successCallback;

  // проверка соответствия типа поля указанному типу
  bool hasType(const nlohmann::json &field, nlohmann::json::value_t type) {
    switch (type) {
    case nlohmann::json::value_t::number_float:
      // любое число
      return field.is_number();
    case nlohmann::json::value_t::number_integer:
      // любое целое число
      return field.is_number_integer();
    case nlohmann::json::value_t::number_unsigned:
      return field.is_number_unsigned();
    }
    return field.type() == type;
  }

public:
  JsonFieldInfo(const std::string &name_,
                const std::function<void(const T &)> successCallback_,
                nlohmann::json::value_t type_,
                const std::function<std::string(const T &)> &checkFn_ = nullptr)
      : name(name_), successCallback(successCallback_), type(type_),
        checkFn(checkFn_) {}

  virtual ~JsonFieldInfo() = default;

  virtual std::optional<std::string> parse(const nlohmann::json &json) {
    std::string nameQuoted = getName(true);

    auto fieldJson = json[name];
    if (!hasType(fieldJson, type))
      return nameQuoted + " must have a type '" + jsonTypeToStr(type) + "'";

    T field = fieldJson.get<T>();

    if (checkFn) {
      std::string checkResult = checkFn(field);
      if (!checkResult.empty())
        return checkResult;
    }

    successCallback(field);
    return std::nullopt;
  }

  virtual std::string getName(bool quoted = false) const override {
    return quoted ? "'" + name + "'" : name;
  }
};
} // namespace common
