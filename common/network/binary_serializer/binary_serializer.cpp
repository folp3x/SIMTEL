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
  size_t lenDiff = common::constants::IMEI_DEFAULT_LENGTH - imei.length();
  if (lenDiff > 0) {
    imei = std::string(lenDiff, '0') + imei;
  }

  if (!Validator::isCorrectImei(imei).empty()) {
    return std::nullopt;
  }

  return imei;
}

std::optional<binary_t> BinarySerializer::imsiToBinary(const imsi_t &imsi) {
  auto imsiNum = fromString<uint64_t>(imsi);
  if (!imsiNum) {
    return std::nullopt;
  }
  return toBinary<>(*imsiNum);
}

std::optional<imsi_t> BinarySerializer::imsiFromBinary(const binary_t &binary) {
  auto deserialized = fromBinary<uint64_t>(binary);
  if (!deserialized) {
    return std::nullopt;
  }

  imsi_t imsi = imsiToStr(*deserialized);
  if (!Validator::isCorrectImsi(imsi).empty()) {
    return std::nullopt;
  }

  return imsi;
}

std::optional<binary_t> BinarySerializer::msisdnToBinary(const msisdn_t &imsi) {
  auto msisdnNum = fromString<uint64_t>(imsi);
  if (!msisdnNum) {
    return std::nullopt;
  }
  return toBinary<>(*msisdnNum);
}

std::optional<msisdn_t>
BinarySerializer::msisdnFromBinary(const binary_t &binary) {
  auto deserialized = fromBinary<uint64_t>(binary);
  if (!deserialized) {
    return std::nullopt;
  }

  return std::to_string(*deserialized);
}
} // namespace common
