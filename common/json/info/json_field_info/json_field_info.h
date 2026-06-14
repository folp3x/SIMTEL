#pragma once

#include <functional>

#include "common/json/info/json_base_info/json_base_info.h"
#include "common/json/json_type/json_type.h"
#include "common/logging/logger/logger.h"

namespace common {
// класс с информацией для парсинга простого JSON-поля
template <typename T> class JsonFieldInfo : public JsonBaseInfo {
private:
  const nlohmann::json::value_t type;
  const std::function<std::string(const T &)> checkFn;

  template <typename U>
  static nlohmann::json::value_t recognizeType(bool arrayType);

protected:
  const std::string name = "";
  T *field;
  const std::function<void(const T &)> successCallback;

  void logConstructor(const std::string &constructorType,
                      const std::string &name) const;

  virtual std::string getName(bool quoted = false) const override;

  virtual nlohmann::json getFieldJson(const nlohmann::json &json) const;

public:
  JsonFieldInfo(const std::string &name_,
                const std::function<void(const T &)> &successCallback_,
                const std::function<std::string(const T &)> &checkFn_ = nullptr,
                bool arrayType = false);

  JsonFieldInfo(const JsonFieldInfo &other);

  JsonFieldInfo(JsonFieldInfo &&other) noexcept;

  virtual std::optional<std::string> parse(const nlohmann::json &json,
                                           bool finalParse = true) override;
};
} // namespace common

#include "json_field_info_impl.h"
