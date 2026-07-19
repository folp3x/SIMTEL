#pragma once

namespace common {
template <typename Container>
JsonContainerInfo<Container>::JsonContainerInfo(
    Container *value,
    const std::function<std::string(const T &)> &elemValidateFunc_)
    : JsonValueInfo<Container>(value), elemValidateFunc(elemValidateFunc_) {}

template <typename Container>
std::optional<std::string>
JsonContainerInfo<Container>::parse(const nlohmann::json &json) {
  if (!json.is_array()) {
    return "JSON array expected";
  }

  return parseContainer(json);
}
} // namespace common
