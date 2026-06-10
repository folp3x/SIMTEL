#pragma once

namespace common {
template <typename T>
std::expected<T, std::string>
JsonDeserializer::deserializeValue(const std::string &name,
                                   nlohmann::json::value_t type,
                                   const std::string &jsonStr) {
  T value;
  auto valueInfo = std::make_unique<JsonFieldInfo<T>>(
      name, [&](const T &value_) { value = value_; }, type);

  auto error = JsonParser<T>::parseField(std::move(valueInfo), jsonStr);
  if (error) {
    return std::unexpected(*error);
  }

  return value;
}
} // namespace common
