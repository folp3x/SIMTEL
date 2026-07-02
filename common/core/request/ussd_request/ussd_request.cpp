#include "ussd_request.h"

#include "common/network/binary_iterator/binary_iterator.h"
#include "common/network/json_deserializer/json_deserializer.h"

namespace common {
UssdRequest::UssdRequest(uint8_t code_) : code(code_) {}

RequestType UssdRequest::getType() const { return RequestType::UssdCode; }

nlohmann::json UssdRequest::toJson() const {
  return nlohmann::json{{"code", code}};
}

std::optional<std::string>
UssdRequest::fromJsonStr(const std::string &jsonStr) {
  auto parsedCode = JsonDeserializer::ussdCodeFromJsonStr(jsonStr, "code");
  if (!parsedCode) {
    return parsedCode.error();
  }
  code = *parsedCode;

  return std::nullopt;
}

std::expected<binary_t, std::string> UssdRequest::toBinary() const {
  auto binCode = BinarySerializer::toBinary(code);
  if (!binCode) {
    return std::unexpected("Code serialize error");
  }

  return *binCode;
}

std::optional<std::string> UssdRequest::fromBinary(const binary_t &binary) {
  BinaryIterator it{binary};

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

uint8_t UssdRequest::getCode() const { return code; }
} // namespace common
