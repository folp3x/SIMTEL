#pragma once

#include <array>

#include "config_field_info.h"

namespace common {
// класс с информацией для парсинга JSON-поля с типом массив
template <typename T, size_t S>
class ConfigArrayInfo : public ConfigFieldInfo<std::array<T, S>> {
private:
  nlohmann::json::value_t elemType;

public:
  ConfigArrayInfo(const std::string &name, std::array<T, S> *field,
                  nlohmann::json::value_t elemType_,
                  const std::function<std::string(const std::array<T, S> &)>
                      &checkFn = nullptr)
      : ConfigFieldInfo<std::array<T, S>>(
            name, field, nlohmann::json::value_t::array, checkFn),
        elemType(elemType_) {}

  virtual ~ConfigArrayInfo() = default;

  virtual std::optional<std::string> parse(const nlohmann::json &json) {
    auto error = ConfigFieldInfo<std::array<T, S>>::parse(json);
    if (error)
      return *error;

    std::string nameQuoted = this->getName(true);

    auto fieldJson = json[this->name];
    if (fieldJson.size() != S)
      return nameQuoted + " must have exactly " + std::to_string(S) +
             " elements";

    for (size_t i = 0; i < S; ++i) {
      auto elemJson = json[this->name][i];
      if (!(this->hasType(elemJson, elemType))) {
        return nameQuoted + " elements must have a type";
      }
      (*(this->field))[i] = elemJson.template get<T>();
    }

    return std::nullopt;
  }
};
} // namespace common
