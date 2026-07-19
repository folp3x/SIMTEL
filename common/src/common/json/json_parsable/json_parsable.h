#pragma once

#include <expected>
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>

#include "common/json/info/json_array_info/json_array_info.h"
#include "common/json/info/json_vector_info/json_vector_info.h"

#include "common/json/info/json_object_info/json_object_info.h"
#include "common/json/info/json_repeat_object_info/json_repeat_object_info.h"
#include "common/json/info/json_value_info/json_value_info.h"

namespace common {
class JsonParsable {
protected:
  template <typename T>
  static std::unique_ptr<JsonValueInfo<T>> makeJsonValue(
      T *value,
      const std::function<std::string(const T &)> &validateFunc = nullptr);

  template <typename T, size_t S>
    requires std::is_fundamental_v<T> || std::is_same_v<T, std::string>
  static std::unique_ptr<JsonArrayInfo<T, S>> makeJsonArray(
      std::array<T, S> *value,
      const std::function<std::string(const T &)> &elemValidateFunc = nullptr);

  template <typename T>
    requires std::is_fundamental_v<T> || std::is_same_v<T, std::string>
  static std::unique_ptr<JsonVectorInfo<T>> makeJsonVector(
      std::vector<T> *value,
      const std::function<std::string(const T &)> &elemValidateFunc = nullptr);

  static std::unique_ptr<JsonObjectInfo> makeJsonObject();

  static std::unique_ptr<JsonRepeatObjectInfoInfo>
  makeJsonRepeatObject(std::unique_ptr<JsonObjectInfo> objectInfo,
                       const std::function<void()> &objectCallback);

  virtual std::unique_ptr<BaseJsonInfo> getJsonRootInfo() = 0;

public:
  virtual ~JsonParsable() = default;

  std::optional<std::string> fromJson(const nlohmann::json &json);

  std::optional<std::string> fromJsonStr(const std::string str);

  std::optional<std::string> fromJsonFile(const std::string &filePath);
};
} // namespace common

#include "json_parsable_impl.h"
