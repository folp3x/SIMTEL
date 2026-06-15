#pragma once

#include "common/json/json_parser/json_parser.h"
#include "common/types.h"

namespace common {
class JsonDeserializer {
private:
  template <typename T>
  static std::expected<T, std::string> deserializeValue(
      const std::string &name, const std::string &jsonStr,
      const std::function<std::string(const T &)> &checkFn = nullptr);

public:
  static std::expected<imei_t, std::string>
  imeiFromJsonStr(const std::string &str, const std::string &name);

  static std::expected<imsi_t, std::string>
  imsiFromJsonStr(const std::string &str, const std::string &name);

  static std::expected<unsigned int, std::string>
  signalFromJsonStr(const std::string &str, const std::string &name);

  static std::expected<unsigned int, std::string>
  bsIdFromJsonStr(const std::string &str, const std::string &name);

  static std::expected<unsigned int, std::string>
  smsIdFromJsonStr(const std::string &str, const std::string &name);

  static std::expected<std::string, std::string>
  strFromJsonStr(const std::string &str, const std::string &name);
};
} // namespace common

#include "json_deserializer_impl.h"
