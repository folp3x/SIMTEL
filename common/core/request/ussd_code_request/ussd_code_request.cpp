#include "ussd_code_request.h"

#include "common/network/binary_iterator/binary_iterator.h"
#include "common/network/json_deserializer/json_deserializer.h"
#include "common/utils/network/network.h"

namespace common {
UssdCodeRequest::UssdCodeRequest(const imsi_t &mTimsi_, uint8_t code_)
    : mTimsi(mTimsi_), code(code_) {}

RequestType UssdCodeRequest::getType() const { return RequestType::UssdCode; }

nlohmann::json UssdCodeRequest::toJson() const {
  return nlohmann::json{{"mTimsi", mTimsi}, {"code", code}};
}

std::optional<std::string>
UssdCodeRequest::fromJsonStr(const std::string &jsonStr) {
  auto parsedMTimsi = JsonDeserializer::imsiFromJsonStr(jsonStr, "mTimsi");
  if (!parsedMTimsi) {
    return parsedMTimsi.error();
  }
  mTimsi = *parsedMTimsi;

  auto parsedCode = JsonDeserializer::ussdCodeFromJsonStr(jsonStr, "code");
  if (!parsedCode) {
    return parsedCode.error();
  }
  code = *parsedCode;

  return std::nullopt;
}

std::expected<binary_t, std::string> UssdCodeRequest::toBinary() const {
  auto binMTimsi = BinarySerializer::imsiToBinary(mTimsi);
  if (!binMTimsi) {
    return std::unexpected("m-timsi serialize error");
  }

  auto binCode = BinarySerializer::toBinary(code);
  if (!binCode) {
    return std::unexpected("Code serialize error");
  }

  return mergeBinary(*binMTimsi, *binCode);
}

std::optional<std::string> UssdCodeRequest::fromBinary(const binary_t &binary) {
  BinaryIterator it{binary};

  auto mTimsiBinary = it.getNext(constants::IMSI_BINARY_BYTES);
  if (!mTimsiBinary) {
    return "Binary too short for m-TIMSI";
  }
  auto parsedMTimsi = BinarySerializer::imsiFromBinary(*mTimsiBinary);
  if (!parsedMTimsi) {
    return "m-TIMSI deserialize error";
  }
  mTimsi = *parsedMTimsi;

  auto codeBinary = it.getNext(sizeof(code));
  if (!codeBinary) {
    return "Binary too short for code";
  }
  auto parsedCode = BinarySerializer::fromBinary<uint8_t>(*codeBinary);
  if (!parsedCode) {
    return "Code deserialize error";
  }
  code = *parsedCode;

  return std::nullopt;
}

imsi_t UssdCodeRequest::getMTimsi() const { return mTimsi; }

uint8_t UssdCodeRequest::getCode() const { return code; }
} // namespace common
