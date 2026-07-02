#include "json_deserializer.h"

#include "common/validator/validator.h"

namespace common {
std::expected<imei_t, std::string>
JsonDeserializer::imeiFromJsonStr(const std::string &str,
                                  const std::string &name) {
  return deserializeValue<imei_t>(name, str, common::Validator::isCorrectImei);
}

std::expected<imsi_t, std::string>
JsonDeserializer::imsiFromJsonStr(const std::string &str,
                                  const std::string &name) {
  return deserializeValue<imsi_t>(name, str, common::Validator::isCorrectImsi);
}

std::expected<unsigned int, std::string>
JsonDeserializer::signalFromJsonStr(const std::string &str,
                                    const std::string &name) {
  return deserializeValue<unsigned int>(name, str,
                                        common::Validator::isCorrectSignal);
}

std::expected<unsigned int, std::string>
JsonDeserializer::bsIdFromJsonStr(const std::string &str,
                                  const std::string &name) {
  return deserializeValue<unsigned int>(name, str);
}

std::expected<unsigned int, std::string>
JsonDeserializer::smsIdFromJsonStr(const std::string &str,
                                   const std::string &name) {
  return deserializeValue<unsigned int>(name, str);
}

std::expected<uint8_t, std::string>
JsonDeserializer::ussdCodeFromJsonStr(const std::string &str,
                                      const std::string &name) {
  return deserializeValue<uint8_t>(name, str);
}

std::expected<double, std::string>
JsonDeserializer::balanceFromJsonStr(const std::string &str,
                                     const std::string &name) {
  return deserializeValue<double>(name, str);
}

std::expected<std::string, std::string>
JsonDeserializer::strFromJsonStr(const std::string &str,
                                 const std::string &name) {
  return deserializeValue<std::string>(name, str);
}
} // namespace common
