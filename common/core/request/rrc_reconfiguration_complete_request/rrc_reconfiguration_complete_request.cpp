#include "rrc_reconfiguration_complete_request.h"

#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/str/str.h"

namespace common {
RrcReconfigurationCompleteRequest::RrcReconfigurationCompleteRequest(
    const imsi_t &mTimsi_)
    : mTimsi(mTimsi_) {}

RequestType RrcReconfigurationCompleteRequest::getType() const {
  return RequestType::RrcReconfigurationComplete;
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
  binary_t binary;
  BinarySerializer::addToBinary(binary, fromStringSafe<uint64_t>(mTimsi));
  return binary;
}

std::optional<std::string>
RrcReconfigurationCompleteRequest::fromBinary(const binary_t &binary) {
  binary_t mTimsiBinary(binary.begin(), binary.end());
  auto parsedMTimsi = BinarySerializer::fromBinary<uint64_t>(mTimsiBinary);
  if (!parsedMTimsi) {
    return "m-TIMSI deserialize error";
  }
  mTimsi = imsiToStr(*parsedMTimsi);

  return std::nullopt;
}

imsi_t RrcReconfigurationCompleteRequest::getMTimsi() const { return mTimsi; }
} // namespace common
