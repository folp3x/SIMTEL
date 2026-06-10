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
  static std::expected<common::imei_t, std::string>
  imeiFromJsonStr(const std::string &str);

  static std::expected<common::imsi_t, std::string>
  imsiFromJsonStr(const std::string &str);

  static std::expected<unsigned int, std::string>
  signalFromJsonStr(const std::string &str);

  static std::expected<unsigned int, std::string>
  bsIdFromJsonStr(const std::string &str);
};
} // namespace common

#include "json_deserializer_impl.h"
