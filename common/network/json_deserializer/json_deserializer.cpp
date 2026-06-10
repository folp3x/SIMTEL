#include "json_deserializer.h"

namespace common {
std::expected<common::imei_t, std::string>
JsonDeserializer::imeiFromJsonStr(const std::string &str) {
  return deserializeValue<common::imei_t>("imei",
                                          nlohmann::json::value_t::string, str);
}

std::expected<common::imsi_t, std::string>
JsonDeserializer::imsiFromJsonStr(const std::string &str) {
  return deserializeValue<common::imsi_t>("imsi",
                                          nlohmann::json::value_t::string, str);
}

std::expected<unsigned int, std::string>
JsonDeserializer::signalFromJsonStr(const std::string &str) {
  return deserializeValue<unsigned int>(
      "signal", nlohmann::json::value_t::number_unsigned, str);
}

std::expected<unsigned int, std::string>
JsonDeserializer::bsIdFromJsonStr(const std::string &str) {
  return deserializeValue<unsigned int>(
      "bsId", nlohmann::json::value_t::number_unsigned, str);
}
} // namespace common
