#pragma once

#include "common/json/info/json_container_info/json_container_info.h"

#include <array>

namespace common {
// класс с информацией для парсинга масства из JSON
template <typename T, size_t S>
class JsonArrayInfo : public JsonContainerInfo<std::array<T, S>> {
private:
  const nlohmann::json::value_t elemType;

  virtual std::expected<std::array<T, S>, std::string>
  parseContainer(const nlohmann::json &fieldJson) override;

public:
  JsonArrayInfo(
      const std::string &name,
      const std::function<void(const std::array<T, S> &)> &successCallback,
      nlohmann::json::value_t elemType_,
      const std::function<std::string(const std::array<T, S> &)> &checkFn =
          nullptr);
};
} // namespace common

#include "json_array_info_impl.h"
