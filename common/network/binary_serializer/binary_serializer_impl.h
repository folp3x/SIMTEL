#pragma once

#include <zpp_bits.h>

namespace common {
template <typename T>
std::optional<std::string> BinarySerializer::addToBinary(binary_t &binary,
                                                         const T &value) {
  zpp::bits::out out(binary, zpp::bits::endian::big{});
  bool error;
  if constexpr (std::is_same_v<T, std::string>) {
    std::string_view view = value;
    error = failure(out(zpp::bits::sized<std::uint32_t>(view)));
  } else {
    error = zpp::bits::failure(out(value));
  }

  if (error) {
    return "Serialization error at byte " + std::to_string(out.position());
  }

  return std::nullopt;
}

template <typename T>
std::expected<T, std::string>
BinarySerializer::fromBinary(const binary_t &binary) {
  T value;
  auto in = zpp::bits::in(binary, zpp::bits::options::endian::big{});

  if (zpp::bits::failure(in(value))) {
    return std::unexpected("Deserialization error at byte " +
                           std::to_string(in.position()));
  }

  return value;
}
} // namespace common
