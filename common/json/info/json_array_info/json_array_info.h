#pragma once

#include "common/json/info/json_container_info/json_container_info.h"

#include <array>

#include "common/utils/str/str.h"

namespace common {
// класс с информацией для парсинга масства из JSON
template <typename T, size_t S>
class JsonArrayInfo : public JsonContainerInfo<std::array<T, S>> {
private:
  virtual std::optional<std::string>
  parseContainer(const nlohmann::json &json) override;

public:
  JsonArrayInfo(
      std::array<T, S> *value,
      const std::function<std::string(const T &)> &elemValidateFunc = nullptr);
};
} // namespace common

#include "json_array_info_impl.h"
