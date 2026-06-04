#pragma once

#include <fstream>

#include "common/json/info/json_array_info/json_array_info.h"
#include "common/json/info/json_vector_info/json_vector_info.h"

namespace common {
template <typename T>
std::optional<std::string>
JsonParser<T>::parseFields(const nlohmann::json &json) {
  for (const auto &info : fieldsInfo) {
    auto error = info->parse(json);
    if (error) {
      return error;
    }
  }

  return std::nullopt;
}

// добавление в список полей простого JSON-поля
template <typename T>
template <typename F>
void JsonParser<T>::addParsedField(
    const std::string &name,
    const std::function<void(const F &)> &successCallback,
    nlohmann::json::value_t type,
    const std::function<std::string(const F &)> &checkFn) {
  auto info =
      std::make_unique<JsonFieldInfo<F>>(name, successCallback, type, checkFn);
  fieldsInfo.push_back(std::move(info));
}

// добавление в список полей JSON-массива
template <typename T>
template <typename E, size_t S>
void JsonParser<T>::addParsedArray(
    const std::string &name,
    const std::function<void(const std::array<E, S> &)> &successCallback,
    nlohmann::json::value_t elemType,
    const std::function<std::string(const std::array<E, S> &)> &checkFn) {
  auto info = std::make_unique<JsonArrayInfo<E, S>>(name, successCallback,
                                                    elemType, checkFn);
  fieldsInfo.push_back(std::move(info));
}

// добавление в список полей вектора JSON
template <typename T>
template <typename E>
void JsonParser<T>::addParsedVector(
    const std::string &name,
    const std::function<void(const std::vector<E> &)> &successCallback,
    nlohmann::json::value_t elemType,
    const std::function<bool(const E &)> &filterFn,
    const std::function<std::string(const std::vector<E> &)> &checkFn) {
  auto info = std::make_unique<JsonVectorInfo<E>>(name, successCallback,
                                                  elemType, filterFn, checkFn);
  fieldsInfo.push_back(std::move(info));
}

template <typename T>
std::optional<std::string>
JsonParser<T>::parseField(const std::unique_ptr<JsonFieldInfo<T>> &fieldInfo,
                          const std::string &str) {
  try {
    auto json = nlohmann::json::parse(str);
    auto error = fieldInfo->parse(json);
    if (error) {
      return error;
    }
  } catch (const nlohmann::json::exception &e) {
    return "JSON parse error: " + std::string(e.what());
  }

  return std::nullopt;
}

template <typename T>
std::expected<T, std::string>
JsonParser<T>::parse(const std::string &filePath) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    return std::unexpected("Can't open file '" + filePath + "'");
  }

  nlohmann::json json;
  try {
    file >> json;

    if (file.fail()) {
      return std::unexpected("File error");
    }

    return parseJson(json);
  } catch (const nlohmann::json::exception &e) {
    return std::unexpected("JSON parse error: " + std::string(e.what()));
  }
}
} // namespace common
