#pragma once

namespace common {
template <typename T>
JsonVectorInfo<T>::JsonVectorInfo(
    const std::string &name,
    const std::function<void(const std::vector<T> &)> &successCallback,
    nlohmann::json::value_t elemType_,
    const std::function<std::string(const std::vector<T> &)> &checkFn)
    : JsonFieldInfo<std::vector<T>>(name, successCallback,
                                    nlohmann::json::value_t::array, checkFn),
      elemType(elemType_) {}

template <typename T>
std::optional<std::string> JsonVectorInfo<T>::parse(const nlohmann::json &json,
                                                    bool finalParse) {
  auto error = JsonFieldInfo<std::vector<T>>::parse(json, false);
  if (error) {
    return error;
  }

  std::string nameQuoted = this->getName(true);

  const auto &fieldJson = this->getFieldJson(json);

  std::vector<T> field{};

  for (size_t i = 0; i < fieldJson.size(); ++i) {
    auto elemJson = fieldJson[i];
    if (!(hasJsonType(elemJson, elemType))) {
      return nameQuoted + " elements must have a type: '" +
             jsonTypeToStr(elemType) + "'";
    }
    T elem = elemJson.template get<T>();
    field.push_back(elem);
  }

  if (finalParse) {
    this->successCallback(field);
  }

  return std::nullopt;
}
} // namespace common
