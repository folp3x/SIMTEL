#pragma once

#include <array>

#include "common/json/info/json_field_info/json_field_info.h"

namespace common {
// класс с информацией для парсинга JSON-поля с типом массив
template <typename T, size_t S>
class JsonArrayInfo : public JsonFieldInfo<std::array<T, S>> {
private:
  const nlohmann::json::value_t elemType;

  void logConstructor(const std::string &constructorType,
                      const std::string &name,
                      nlohmann::json::value_t elemType) const {
    SPDLOG_LOGGER_DEBUG(
        spdlog::default_logger(),
        "common::JsonArrayInfo {} constructor called: name={}, type={}",
        constructorType, name, jsonTypeToStr(elemType));
  }

public:
  JsonArrayInfo(
      const std::string &name,
      const std::function<void(const std::array<T, S> &)> successCallback,
      nlohmann::json::value_t elemType_,
      const std::function<std::string(const std::array<T, S> &)> &checkFn =
          nullptr)
      : JsonFieldInfo<std::array<T, S>>(
            name, successCallback, nlohmann::json::value_t::array, checkFn),
        elemType(elemType_) {}

  JsonArrayInfo(const JsonArrayInfo &other)
      : JsonFieldInfo<std::array<T, S>>(other.name, other.successCallback,
                                        other.type, other.checkFn),
        elemType(other.elemType) {
    logConstructor("COPY", this->name, elemType);
  }

  JsonArrayInfo(JsonArrayInfo &&other) noexcept
      : JsonFieldInfo<std::array<T, S>>(std::move(other.name),
                                        std::move(other.successCallback),
                                        other.type, std::move(other.checkFn)),
        elemType(other.elemType) {
    logConstructor("MOVE", this->name, elemType);
  }

  virtual ~JsonArrayInfo() = default;

  virtual std::optional<std::string> parse(const nlohmann::json &json) {
    auto error = JsonFieldInfo<std::array<T, S>>::parse(json);
    if (error)
      return *error;

    std::string nameQuoted = this->getName(true);

    auto fieldJson = json[this->name];
    if (fieldJson.size() != S)
      return nameQuoted + " must have exactly " + std::to_string(S) +
             " elements";

    std::array<T, S> field{};

    for (size_t i = 0; i < S; ++i) {
      auto elemJson = json[this->name][i];
      if (!(this->hasType(elemJson, elemType))) {
        return nameQuoted + " elements must have a type '" +
               jsonTypeToStr(elemType) + "'";
      }
      field[i] = elemJson.template get<T>();
    }

    this->successCallback(field);
    return std::nullopt;
  }
};
} // namespace common
