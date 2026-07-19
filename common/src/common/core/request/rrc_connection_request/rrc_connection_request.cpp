#include "rrc_connection_request.h"

#include "common/utils/str/str.h"
#include "common/validator/validator.h"

namespace common {
RrcConnectionRequest::RrcConnectionRequest(const imei_t &imei_,
                                           const coords_t<> &coords_)
    : imei(imei_), coords(coords_) {}

RequestType RrcConnectionRequest::getType() const {
  return RequestType::RrcConnection;
}

nlohmann::json RrcConnectionRequest::toJson() const {
  return nlohmann::json{{"loc", coords}, {"imei", imei}};
}

std::unique_ptr<BaseJsonInfo> RrcConnectionRequest::getJsonRootInfo() {
  auto root = makeJsonObject();

  root->addInner("imei",
                 makeJsonValue<imei_t>(&imei, Validator::isCorrectImei));

  root->addInner("loc", makeJsonArray(&coords));

  return root;
}

std::vector<std::unique_ptr<BaseBinaryInfo>>
RrcConnectionRequest::getBinaryValuesInfo() {
  std::vector<std::unique_ptr<BaseBinaryInfo>> valuesInfo{};

  valuesInfo.emplace_back(makeBinaryValue<imei_t, uint64_t>(
      &imei, Validator::isCorrectImei, utils::identifierFromStr,
      utils::imeiToStr));

  valuesInfo.emplace_back(makeBinaryValue(&coords));

  return valuesInfo;
}

imei_t RrcConnectionRequest::getImei() const { return imei; }

coords_t<> RrcConnectionRequest::getCoords() const { return coords; }
} // namespace common
