#pragma once

namespace common {
template <typename T>
std::expected<T, std::string> JsonDeserializer::deserializeValue(
    const std::string &name, const std::string &jsonStr,
    const std::function<std::string(const T &)> &checkFn) {
  T value;
  auto valueInfo = std::make_unique<JsonFieldInfo<T>>(
      name, [&](const T &value_) { value = value_; }, checkFn);

  auto error = JsonParser<T>::parseField(std::move(valueInfo), jsonStr);
  if (error) {
    return std::unexpected(*error);
  }

  return value;
}
} // namespace common
