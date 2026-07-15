#include "ussd_msisdn_response.h"

#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/str/str.h"

namespace common {
UssdMsisdnResponse::UssdMsisdnResponse(const msisdn_t &msisdn_)
    : msisdn(msisdn_) {}

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
  binary_t binary;
  BinarySerializer::addToBinary(binary,
                                utils::fromStringSafe<uint64_t>(msisdn));

  return binary;
}

std::optional<std::string>
UssdMsisdnResponse::fromBinary(const binary_t &binary) {
  auto msisdnBin = BinarySerializer::fromBinary<uint64_t>(binary);
  if (!msisdnBin) {
    return "Failed to deserialize msisdn";
  }
  msisdn = *msisdnBin;

  return std::nullopt;
}

msisdn_t UssdMsisdnResponse::getMsisdn() const { return msisdn; }
} // namespace common
