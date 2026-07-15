#pragma once

#include <memory>

#include "common/binary/binary_value_info/binary_value_info.h"

namespace common {
class BinarySerializable {
protected:
  virtual std::vector<std::unique_ptr<BaseBinaryInfo>>
  getBinaryValuesInfo() = 0;

  template <typename T, typename S = T>
  std::unique_ptr<BinaryValueInfo<T, S>> makeBinaryValue(
      T *value,
      const std::function<std::string(const T &)> &validateFunc = nullptr,
      const std::function<std::optional<S>(const T &)> &serializeFunc = nullptr,
      const std::function<std::optional<T>(const S &)> &deserializeFunc =
          nullptr);

public:
  std::optional<std::string> fromBinary(const binary_t &binary);

  std::expected<common::binary_t, std::string> toBinary();
};
} // namespace common

#include "binary_serializable_impl.h"
