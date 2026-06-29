#pragma once

#include <concepts>
#include <functional>

#include "common/json/info/json_base_info/json_base_info.h"
#include "common/json/json_type/json_type.h"

namespace common {
// класс с информацией для парсинга простого JSON-поля
template <typename T> class JsonFieldInfo : public JsonBaseInfo {
private:
  const nlohmann::json::value_t type;
  const std::function<std::string(const T &)> checkFn;

protected:
  const std::function<void(const T &)> successCallback;

  template <typename U>
  static nlohmann::json::value_t recognizeType(bool arrayType = false);

public:
  JsonFieldInfo(const std::string &name,
                const std::function<void(const T &)> &successCallback_,
                const std::function<std::string(const T &)> &checkFn_ = nullptr,
                bool arrayType = false);

  virtual std::optional<std::string> parse(const nlohmann::json &json,
                                           bool finalParse = true) override;
};
} // namespace common

#include "json_field_info_impl.h"
