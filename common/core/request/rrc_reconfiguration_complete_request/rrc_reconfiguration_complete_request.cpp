#include "rrc_reconfiguration_complete_request.h"

#include "common/network/binary_serializer/binary_serializer.h"
#include "common/network/json_deserializer/json_deserializer.h"

namespace common {
RrcReconfigurationCompleteRequest::RrcReconfigurationCompleteRequest(
    const imsi_t &mTimsi_)
    : mTimsi(mTimsi_) {}

RequestType RrcReconfigurationCompleteRequest::getType() const {
  return RequestType::Rrc_Reconfiguration_Complete;
}

nlohmann::json RrcReconfigurationCompleteRequest::toJson() const {
  return nlohmann::json{{"mTimsi", mTimsi}};
}

std::optional<std::string>
RrcReconfigurationCompleteRequest::fromJsonStr(const std::string &jsonStr) {
  auto parsedMTimsi = JsonDeserializer::imsiFromJsonStr(jsonStr, "mTimsi");
  if (!parsedMTimsi) {
    return parsedMTimsi.error();
  }
  mTimsi = *parsedMTimsi;

  return std::nullopt;
}

std::expected<binary_t, std::string>
RrcReconfigurationCompleteRequest::toBinary() const {
  auto binMTimsi = BinarySerializer::imsiToBinary(mTimsi);
  if (!binMTimsi) {
    return std::unexpected("m-TIMSI serialize error");
  }

  return *binMTimsi;
}

std::optional<std::string>
RrcReconfigurationCompleteRequest::fromBinary(const binary_t &binary) {
  binary_t mTimsiBinary(binary.begin(), binary.end());
  auto parsedMTimsi = BinarySerializer::imsiFromBinary(mTimsiBinary);
  if (!parsedMTimsi) {
    return "m-TIMSI deserialize error";
  }
  mTimsi = *parsedMTimsi;

  return std::nullopt;
}

imsi_t RrcReconfigurationCompleteRequest::getMTimsi() const { return mTimsi; }
} // namespace common
