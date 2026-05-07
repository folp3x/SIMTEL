#pragma once

#include <functional>
#include <nlohmann/json.hpp>

#include "config_field_base.h"

namespace common {
// класс с информацией для парсинга простого JSON-поля
template <typename F> class ConfigFieldInfo : public ConfigFieldBase {
private:
  nlohmann::detail::value_t type;
  std::function<std::string(const F &)> checkFn;

protected:
  std::string name = "";
  F *field;

  // проверка соответствия типа поля указанному типу
  bool numberTypeEqual(const nlohmann::json &field,
                       nlohmann::detail::value_t t) {
    switch (t) {
    case nlohmann::detail::value_t::number_float:
      return field.is_number();
    case nlohmann::detail::value_t::number_integer:
      return field.is_number_integer();
    case nlohmann::detail::value_t::number_unsigned:
      return field.is_number_unsigned();
    }
    return false;
  }

public:
  ConfigFieldInfo(
      const std::string &name_, F *field_, nlohmann::detail::value_t type_,
      const std::function<std::string(const F &)> &checkFn_ = nullptr)
      : name(name_), field(field_), type(type_), checkFn(checkFn_) {}

  virtual ~ConfigFieldInfo() = default;

  virtual bool parse(const nlohmann::json &json, std::string &msg) {
    std::string nameQuoted = getName(true);

    bool isCorrectType = true;
    if (json[name].is_number()) {
      isCorrectType = numberTypeEqual(json[name], type);
    } else {
      isCorrectType = json[name].type() == type;
    }

    if (!isCorrectType) {
      msg = nameQuoted + " must have a type";
      return false;
    }

    *field = json[name].get<F>();

    if (checkFn) {
      std::string checkResult = checkFn(*field);
      if (!checkResult.empty()) {
        msg = checkResult;
        return false;
      }
    }

    return true;
  }

  virtual std::string getName(bool quoted = false) const override {
    return quoted ? "'" + name + "'" : name;
  }
};
} // namespace common
