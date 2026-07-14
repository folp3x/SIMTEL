#pragma once

#include <concepts>
#include <expected>
#include <optional>

#include "common/types.h"

namespace common {
class BinarySerializer {
public:
  template <typename T>
  static std::optional<std::string> addToBinary(binary_t &binary,
                                                const T &value);

  template <typename T>
  static std::expected<T, std::string> fromBinary(const binary_t &binary);

  static binary_t strToBinaryUnsized(const std::string &binary);
  static std::string strFromBinaryUnsized(const binary_t &binary);
};
} // namespace common

#include "binary_serializer_impl.h"
