#include "binary_serializer.h"

#include "common/utils/str/str.h"
#include "common/validator/validator.h"

namespace common {
binary_t BinarySerializer::strToBinary(const std::string &binary) {
  binary_t result;
  result.reserve(binary.size());
  for (char c : binary) {
    result.push_back(static_cast<std::byte>(c));
  }
  return result;
}

std::string BinarySerializer::strFromBinary(const binary_t &binary) {
  std::string result;
  result.reserve(binary.size());
  for (std::byte b : binary) {
    result.push_back(static_cast<char>(b));
  }
  return result;
}

std::optional<binary_t> BinarySerializer::imeiToBinary(const imei_t &imei) {
  auto imeiNum = fromString<uint64_t>(imei);
  if (!imeiNum) {
    return std::nullopt;
  }
  return toBinary<>(*imeiNum);
}

std::optional<imei_t> BinarySerializer::imeiFromBinary(const binary_t &binary) {
  auto deserialized = fromBinary<uint64_t>(binary);
  if (!deserialized) {
    return std::nullopt;
  }

  imei_t imei = std::to_string(*deserialized);
  size_t lenDiff = IMEI_DEFAULT_LEGNTH - imei.length();
  if (lenDiff > 0) {
    imei = std::string(lenDiff, '0') + imei;
  }

  if (!Validator::isCorrectIMEI(imei).empty()) {
    return std::nullopt;
  }

  return imei;
}
} // namespace common
