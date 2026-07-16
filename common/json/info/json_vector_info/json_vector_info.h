#pragma once

#include "common/json/info/json_container_info/json_container_info.h"

#include <vector>

#include "common/utils/str/str.h"

namespace common {
// класс с информацией для парсинга вектора из JSON
template <typename T>
class JsonVectorInfo : public JsonContainerInfo<std::vector<T>> {
private:
  virtual std::optional<std::string>
  parseContainer(const nlohmann::json &json) override;

public:
  JsonVectorInfo(
      std::vector<T> *value,
      const std::function<std::string(const T &)> &elemValidateFunc = nullptr);
};
} // namespace common

#include "json_vector_info_impl.h"
