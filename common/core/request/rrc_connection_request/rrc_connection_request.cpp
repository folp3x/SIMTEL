#include "rrc_connection_request.h"

#include "common/network/binary_iterator/binary_iterator.h"
#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/str/str.h"

namespace common {
RrcConnectionRequest::RrcConnectionRequest(const imei_t &imei_,
                                           const Location<> &loc_)
    : imei(imei_), loc(loc_) {}

RequestType RrcConnectionRequest::getType() const {
  return RequestType::RrcConnection;
}

nlohmann::json RrcConnectionRequest::toJson() const {
  return nlohmann::json{{"loc", loc.getCoords()}, {"imei", imei}};
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
  binary_t binary;
  BinarySerializer::addToBinary(binary, loc.getCoords());
  BinarySerializer::addToBinary(binary, utils::fromStringSafe<uint64_t>(imei));

  return binary;
}

std::optional<std::string>
RrcConnectionRequest::fromBinary(const binary_t &binary) {
  BinaryIterator it{binary};

  auto imeiBinary = it.getNext(constants::ImeiBinaryBytes);
  if (!imeiBinary) {
    return "Binary too short for IMEI";
  }
  auto parsedImei = BinarySerializer::fromBinary<uint64_t>(*imeiBinary);
  if (!parsedImei) {
    return "IMEI deserialize error";
  }
  imei = utils::imeiToStr(*parsedImei);

  auto locBinary = it.getRemaining();
  if (!locBinary) {
    return "Binary too short for location";
  }
  auto parsedLoc = BinarySerializer::fromBinary<coords_t<>>(*locBinary);
  if (!parsedLoc) {
    return parsedLoc.error();
  }
  loc = Location<>(*parsedLoc);

  return std::nullopt;
}

imei_t RrcConnectionRequest::getImei() const { return imei; }

Location<> RrcConnectionRequest::getLoc() const { return loc; }
} // namespace common
