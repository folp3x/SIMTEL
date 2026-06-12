#pragma once

#include <expected>
#include <nlohmann/json.hpp>

#include "common/json/json_parser/json_parser.h"
#include "common/types.h"

namespace common {
class JsonDeserializer {
private:
  template <typename T>
  static std::expected<T, std::string>
  deserializeValue(const std::string &name, nlohmann::json::value_t type,
                   const std::string &jsonStr);

public:
  static std::expected<imei_t, std::string>
  imeiFromJsonStr(const std::string &str, const std::string &name = "imei");

  static std::expected<imsi_t, std::string>
  imsiFromJsonStr(const std::string &str, const std::string &name = "imsi");

  static std::expected<unsigned int, std::string>
  signalFromJsonStr(const std::string &str, const std::string &name = "signal");

  static std::expected<unsigned int, std::string>
  bsIdFromJsonStr(const std::string &str, const std::string &name = "bsId");

  static std::expected<std::string, std::string>
  strFromJsonStr(const std::string &str, const std::string &name);
};
} // namespace common

#include "json_deserializer_impl.h"
