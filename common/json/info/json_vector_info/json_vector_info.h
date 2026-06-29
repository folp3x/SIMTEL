#pragma once

#include "common/json/info/json_container_info/json_container_info.h"

#include <vector>

namespace common {
// класс с информацией для парсинга вектора из JSON
template <typename T>
class JsonVectorInfo : public JsonContainerInfo<std::vector<T>> {
private:
  virtual std::expected<std::vector<T>, std::string>
  parseContainer(const nlohmann::json &fieldJson) override;

public:
  JsonVectorInfo(
      const std::string &name,
      const std::function<void(const std::vector<T> &)> &successCallback,
      const std::function<std::string(const std::vector<T> &)> &checkFn =
          nullptr);
};
} // namespace common

#include "json_vector_info_impl.h"
