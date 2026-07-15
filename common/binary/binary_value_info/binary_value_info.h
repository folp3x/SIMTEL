#pragma once

#include "common/binary/base_binary_info/base_binary_info.h"

#include <expected>
#include <functional>

namespace common {
template <typename T, typename S = T>
class BinaryValueInfo : public BaseBinaryInfo {
private:
  T *value;

  const std::function<std::string(const T &)> validateFunc;

  const std::function<std::optional<S>(const T &)> serializeFunc;
  const std::function<std::optional<T>(const S &)> deserializeFunc;

  template <typename V>
  static std::expected<V, std::string> valueFromBinary(in_stream_t &stream);

  template <typename V>
  static std::optional<std::string> valueToBinary(out_stream_t &stream,
                                                  const V &value);

  template <typename V>
  static std::expected<V, std::string> parseValue(
      in_stream_t &stream,
      const std::function<std::string(const V &)> &validateFunc = nullptr);

public:
  BinaryValueInfo(
      T *value_,
      const std::function<std::string(const T &)> &validateFunc_ = nullptr,
      const std::function<std::optional<S>(const T &)> &serializeFunc_ =
          nullptr,
      const std::function<std::optional<T>(const S &)> &deserializeFunc_ =
          nullptr);

  virtual std::optional<std::string> parse(in_stream_t &stream) override;

  virtual std::optional<std::string> write(out_stream_t &stream) override;
};
} // namespace common

#include "common/binary/binary_value_info/binary_value_info_impl.h"
