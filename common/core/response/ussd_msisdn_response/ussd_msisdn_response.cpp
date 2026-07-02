#include "ussd_msisdn_response.h"

#include "common/network/json_deserializer/json_deserializer.h"

namespace common {
RequestType UssdMsisdnResponse::getType() const {
  return RequestType::UssdMsisdn;
}

nlohmann::json UssdMsisdnResponse::toJson() const {
  return nlohmann::json{{"msisdn", msisdn}};
}

std::optional<std::string>
UssdMsisdnResponse::fromJsonStr(const std::string &jsonStr) {
  auto parsedMsisdn = JsonDeserializer::strFromJsonStr(jsonStr, "msisdn");
  if (!parsedMsisdn) {
    return parsedMsisdn.error();
  }
  msisdn = *parsedMsisdn;

  return std::nullopt;
}

std::expected<binary_t, std::string> UssdMsisdnResponse::toBinary() const {
  return BinarySerializer::strToBinary(msisdn);
}

std::optional<std::string>
UssdMsisdnResponse::fromBinary(const binary_t &binary) {
  msisdn = BinarySerializer::strFromBinary(binary);

  return std::nullopt;
}

msisdn_t UssdMsisdnResponse::getMsisdn() const { return msisdn; }
} // namespace common
