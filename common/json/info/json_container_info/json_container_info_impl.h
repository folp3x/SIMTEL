#pragma once

namespace common {
template <typename Container>
JsonContainerInfo<Container>::JsonContainerInfo(
    const std::string &name,
    const std::function<void(const Container &)> &successCallback,
    const std::function<std::string(const Container &)> &checkFn)
    : JsonFieldInfo<Container>(name, successCallback, checkFn, true) {}

template <typename Container>
std::optional<std::string>
JsonContainerInfo<Container>::parse(const nlohmann::json &json,
                                    bool finalParse) {
  auto error = JsonFieldInfo<Container>::parse(json, false);
  if (error) {
    return error;
  }

  std::string nameQuoted = this->getName(true);

  nlohmann::json fieldJson = this->getFieldJson(json);
  auto field = parseContainer(fieldJson);
  if (!field) {
    return nameQuoted + ": " + field.error();
  }

  if (finalParse) {
    this->successCallback(*field);
  }

  return std::nullopt;
}
} // namespace common
