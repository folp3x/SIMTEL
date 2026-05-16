#pragma once

#include <array>

#include "common/json/info/json_field_info/json_field_info.h"

namespace common {
// класс с информацией для парсинга JSON-массива
template <typename T, size_t S>
class JsonArrayInfo : public JsonFieldInfo<std::array<T, S>> {
private:
  const nlohmann::json::value_t elemType;

  void logConstructor(const std::string &constructorType,
                      const std::string &name,
                      nlohmann::json::value_t elemType) const;

public:
  JsonArrayInfo(
      const std::string &name,
      const std::function<void(const std::array<T, S> &)> successCallback,
      nlohmann::json::value_t elemType_,
      const std::function<std::string(const std::array<T, S> &)> &checkFn =
          nullptr);

  JsonArrayInfo(const JsonArrayInfo &other);

  JsonArrayInfo(JsonArrayInfo &&other) noexcept;

  virtual std::optional<std::string> parse(const nlohmann::json &json) override;
};
} // namespace common

#include "json_array_info_impl.h"
