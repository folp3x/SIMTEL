#pragma once

#include <concepts>
#include <functional>

#include "common/json/info/base_json_info/base_json_info.h"
#include "common/utils/json/json.h"
#include "common/utils/str/str.h"

namespace common {
// класс с информацией для парсинга простого JSON-поля из объекта
template <typename T> class JsonValueInfo : public BaseJsonInfo {
private:
  const std::function<std::string(const T &)> validateFunc;

protected:
  T *value;

  template <typename U>
  static constexpr std::optional<nlohmann::json::value_t> recognizeType();

public:
  JsonValueInfo(
      T *value_,
      const std::function<std::string(const T &)> &validateFunc_ = nullptr);

  virtual std::optional<std::string> parse(const nlohmann::json &json) override;
};
} // namespace common

#include "json_value_info_impl.h"
