#pragma once

#include <array>

#include "config_field_info.h"

namespace common {
// класс с информацией для парсинга JSON-поля с типом массив
template <typename T, size_t S>
class ConfigArrayInfo : public ConfigFieldInfo<std::array<T, S>> {
private:
  nlohmann::detail::value_t elemType;

public:
  ConfigArrayInfo(const std::string &name, std::array<T, S> *field,
                  nlohmann::detail::value_t elemType_,
                  const std::function<std::string(const std::array<T, S> &)>
                      &checkFn = nullptr)
      : ConfigFieldInfo<std::array<T, S>>(
            name, field, nlohmann::detail::value_t::array, checkFn),
        elemType(elemType_) {}

  virtual ~ConfigArrayInfo() = default;

  virtual bool parse(const nlohmann::json &json, std::string &msg) {
    bool parsed = ConfigFieldInfo<std::array<T, S>>::parse(json, msg);
    if (!parsed)
      return false;

    std::string nameQuoted = this->getName(true);

    if (json[this->name].size() != S) {
      msg =
          nameQuoted + " must have exactly " + std::to_string(S) + " elements";
      return false;
    }

    for (size_t i = 0; i < S; ++i) {
      // проверка типа каждого элемента
      auto elemJson = json[this->name][i];
      bool isCorrectType = true;
      if (elemJson.is_number()) {
        isCorrectType = this->numberTypeEqual(elemJson, elemType);
      } else {
        isCorrectType = elemJson.type() == elemType;
      }

      if (!isCorrectType) {
        msg = nameQuoted + "' elements must have a type";
        return false;
      }
      (*this->field)[i] = elemJson.template get<T>();
    }

    return true;
  }
};
} // namespace common
