#pragma once

namespace common {
template <typename T, typename S>
std::unique_ptr<BinaryValueInfo<T, S>> BinarySerializable::makeBinaryValue(
    T *value, const std::function<std::string(const T &)> &validateFunc,
    const std::function<std::optional<S>(const T &)> &serializeFunc,
    const std::function<std::optional<T>(const S &)> &deserializeFunc) {
  return std::make_unique<BinaryValueInfo<T, S>>(
      value, validateFunc, serializeFunc, deserializeFunc);
}
} // namespace common
