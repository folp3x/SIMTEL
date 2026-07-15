#include "binary_serializable.h"

namespace common {
std::optional<std::string>
BinarySerializable::fromBinary(const binary_t &binary) {
  zpp::bits::in stream(binary, zpp::bits::endian::big{});

  for (const auto &info : getBinaryValuesInfo()) {
    auto error = info->parse(stream);
    if (error) {
      return *error;
    }
  }

  return std::nullopt;
}

std::expected<common::binary_t, std::string> BinarySerializable::toBinary() {
  binary_t binary{};
  zpp::bits::out stream(binary, zpp::bits::endian::big{});

  for (const auto &info : getBinaryValuesInfo()) {
    auto error = info->write(stream);
    if (error) {
      return std::unexpected(*error);
    }
  }

  return binary;
}
} // namespace common
