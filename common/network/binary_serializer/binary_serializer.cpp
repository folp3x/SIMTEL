#include "binary_serializer.h"

#include "common/utils/str/str.h"
#include "common/validator/validator.h"

namespace common {
std::optional<binary_t> BinarySerializer::imeiToBinary(const imei_t &imei) {
  auto toNumConvertResult = fromString<uint64_t>(imei);
  if (!toNumConvertResult) {
    return std::nullopt;
  }

  return toBinary<>(*toNumConvertResult);
}

std::optional<imei_t> BinarySerializer::imeiFromBinary(const binary_t &binary) {
  auto serializeResult = fromBinary<uint64_t>(binary);
  if (!serializeResult) {
    return std::nullopt;
  }

  imei_t imei = std::to_string(*serializeResult);
  size_t lenDiff = constants::IMEI_DEFAULT_LEGNTH - imei.length();
  if (lenDiff > 0) {
    imei = std::string(lenDiff, '0') + imei;
  }

  if (!Validator::isCorrectIMEI(imei).empty()) {
    return std::nullopt;
  }

  return imei;
}
} // namespace common
