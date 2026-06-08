#pragma once

#include <zpp_bits.h>

namespace common {
template <typename T>
  requires std::is_arithmetic_v<T>
std::optional<binary_t> BinarySerializer::toBinary(T data) {
  binary_t binary;
  auto out = zpp::bits::out(binary, zpp::bits::options::endian::big{});

  if (out(data) != zpp::bits::errc{}) {
    return std::nullopt;
  }

  return binary;
}

template <typename T>
  requires std::is_arithmetic_v<T>
std::optional<T> BinarySerializer::fromBinary(const binary_t &binary) {
  T data;
  auto in = zpp::bits::in(std::span(binary), zpp::bits::options::endian::big{});

  if (in(data) != zpp::bits::errc{}) {
    return std::nullopt;
  }

  return data;
}

template <std::ranges::contiguous_range Container>
std::optional<binary_t> BinarySerializer::toBinary(const Container &data) {
  binary_t binary;
  auto out = zpp::bits::out(binary, zpp::bits::options::endian::big{});

  if (out(zpp::bits::unsized(data)) != zpp::bits::errc{}) {
    return std::nullopt;
  }

  return binary;
}

template <std::ranges::contiguous_range Container>
bool BinarySerializer::fromBinary(const binary_t &binary, Container &data) {
  if (std::ranges::empty(data)) {
    throw std::invalid_argument("data cant be empty and must have size equal "
                                "to expected elements count");
  }

  auto in = zpp::bits::in(binary, zpp::bits::options::endian::big{});

  if (in(zpp::bits::unsized(data)) != zpp::bits::errc{}) {
    return false;
  }

  return true;
}
} // namespace common
