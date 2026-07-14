#include "binary_serializer.h"

#include "common/utils/str/str.h"
#include "common/validator/validator.h"

namespace common {
binary_t BinarySerializer::strToBinaryUnsized(const std::string &binary) {
  binary_t result;
  result.reserve(binary.size());
  for (auto byte : binary) {
    result.push_back(static_cast<std::byte>(byte));
  }
  return result;
}

std::string BinarySerializer::strFromBinaryUnsized(const binary_t &binary) {
  std::string result;
  result.reserve(binary.size());
  for (std::byte byte : binary) {
    result.push_back(static_cast<char>(byte));
  }
  return result;
}
} // namespace common
