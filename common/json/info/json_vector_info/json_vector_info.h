#pragma once

#include "common/json/info/json_container_info/json_container_info.h"

#include <vector>

namespace common {
// класс с информацией для парсинга вектора из JSON
template <typename T>
class JsonVectorInfo : public JsonContainerInfo<std::vector<T>> {
private:
  const nlohmann::json::value_t elemType;

public:
  JsonVectorInfo(
      const std::string &name,
      const std::function<void(const std::vector<T> &)> &successCallback,
      nlohmann::json::value_t elemType_,
      const std::function<std::string(const std::vector<T> &)> &checkFn =
          nullptr);

  virtual std::expected<std::vector<T>, std::string>
  parseContainer(const nlohmann::json &fieldJson) override;
};
} // namespace common

#include "json_vector_info_impl.h"
