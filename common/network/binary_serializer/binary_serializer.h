#pragma once

#include <concepts>
#include <optional>

#include "common/types.h"

namespace common {
class BinarySerializer {
public:
  template <typename T>
    requires std::is_arithmetic_v<T>
  static std::optional<binary_t> toBinary(T data);

  template <typename T>
    requires std::is_arithmetic_v<T>
  static std::optional<T> fromBinary(const binary_t &binary);

  // контейнер, элементы которого расположены в памяти непрерывно
  template <std::ranges::contiguous_range Container>
  static std::optional<binary_t> toBinary(const Container &data);

  template <std::ranges::contiguous_range Container>
  static bool fromBinary(const binary_t &binary, Container &data);

  static binary_t strToBinary(const std::string &binary);
  static std::string strFromBinary(const binary_t &binary);

  static std::optional<binary_t> imeiToBinary(const imei_t &imei);

  static std::optional<imei_t> imeiFromBinary(const binary_t &binary);
};
} // namespace common

#include "binary_serializer_impl.h"
