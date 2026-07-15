#include "rrc_connection_request.h"

#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/str/str.h"
#include "common/validator/validator.h"

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

std::vector<std::unique_ptr<BaseBinaryInfo>>
RrcConnectionRequest::getBinaryValuesInfo() {
  std::vector<std::unique_ptr<BaseBinaryInfo>> valuesInfo{};

  valuesInfo.emplace_back(makeBinaryValue<imei_t, uint64_t>(
      &imei, Validator::isCorrectImei, utils::identifierFromStr,
      utils::imeiToStr));

  valuesInfo.emplace_back(makeBinaryValue<Location<>, coords_t<>>(
      &loc, nullptr, [](const Location<> &loc) { return loc.getCoords(); },
      [](const coords_t<> &coords) { return Location<>(coords); }));

  return valuesInfo;
}

imei_t RrcConnectionRequest::getImei() const { return imei; }

Location<> RrcConnectionRequest::getLoc() const { return loc; }
} // namespace common
