#include "rrc_connection_request.h"

#include "common/network/binary_iterator/binary_iterator.h"
#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/network/network.h"

namespace common {
RrcConnectionRequest::RrcConnectionRequest(const imei_t &imei_,
                                           const Location<> &loc_)
    : imei(imei_), loc(loc_) {}

RequestType RrcConnectionRequest::getType() const {
  return RequestType::RrcConnection;
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
RrcConnectionRequest::fromBinary(const binary_t &binary) {
  BinaryIterator it{binary};

  auto imeiBinary = it.getNext(constants::ImeiBinaryBytes);
  if (!imeiBinary) {
    return "Binary too short for IMEI";
  }
  auto parsedImei = BinarySerializer::imeiFromBinary(*imeiBinary);
  if (!parsedImei) {
    return "IMEI deserialize error";
  }
  imei = *parsedImei;

  auto locBinary = it.getRemaining();
  if (!locBinary) {
    return "Binary too short for location";
  }
  auto parsedLoc = Location<>::fromBinary(*locBinary);
  if (!parsedLoc) {
    return parsedLoc.error();
  }
  loc = *parsedLoc;

  return std::nullopt;
}

imei_t RrcConnectionRequest::getImei() const { return imei; }

Location<> RrcConnectionRequest::getLoc() const { return loc; }
} // namespace common
