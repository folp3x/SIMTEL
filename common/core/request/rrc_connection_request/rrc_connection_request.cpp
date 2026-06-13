#include "rrc_connection_request.h"

#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/network/network.h"

namespace common {
RrcConnectionRequest::RrcConnectionRequest(const imei_t &imei_,
                                           const Location<> &loc_)
    : imei(imei_), loc(loc_) {}

std::string RrcConnectionRequest::toStr() const {
  return "Rrc_Connection{imei=" + imei + ", loc=" + loc.toStr() + "}";
}

RequestType RrcConnectionRequest::getType() const {
  return RequestType::Rrc_Connection;
}

nlohmann::json RrcConnectionRequest::toJson() const {
  nlohmann::json jsonObj = loc.toJson();
  jsonObj["imei"] = imei;
  return jsonObj;
}

std::optional<std::string>
RrcConnectionRequest::fromJsonStr(const std::string &jsonStr) {
  auto parsedImei = JsonDeserializer::imeiFromJsonStr(jsonStr, "imei");
  if (!parsedImei) {
    return parsedImei.error();
  }
  imei = *parsedImei;

  auto parsedLoc = Location<>::fromJsonStr(jsonStr);
  if (!parsedLoc) {
    return parsedLoc.error();
  }
  loc = *parsedLoc;

  return std::nullopt;
}

std::expected<binary_t, std::string> RrcConnectionRequest::toBinary() const {
  auto binLoc = loc.toBinary();
  if (!binLoc) {
    return std::unexpected(binLoc.error());
  }

  auto binImei = BinarySerializer::imeiToBinary(imei);
  if (!binImei) {
    return std::unexpected("IMEI serialize error");
  }

  return mergeBinary(*binImei, *binLoc);
}

std::optional<std::string>
RrcConnectionRequest::fromBinary(const common::binary_t &binary) {
  auto curByte = binary.begin();

  if (curByte + common::constants::IMEI_BINARY_BYTES > binary.end()) {
    return "Binary too short for IMEI";
  }
  binary_t imeiBinary(curByte, curByte + common::constants::IMEI_BINARY_BYTES);
  auto parsedImei = BinarySerializer::imeiFromBinary(imeiBinary);
  if (!parsedImei) {
    return "IMEI deserialize error";
  }
  imei = *parsedImei;
  curByte += common::constants::IMEI_BINARY_BYTES;

  auto parsedLoc = Location<>::fromBinary(binary_t{curByte, binary.end()});
  if (!parsedLoc) {
    return parsedLoc.error();
  }
  loc = *parsedLoc;

  return std::nullopt;
}
} // namespace common
