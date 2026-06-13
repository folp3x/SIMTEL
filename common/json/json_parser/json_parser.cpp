#include "json_parser.h"

#include "common/json/info/json_object_info/json_object_info.h"

namespace common {
template <typename T>
void JsonParser<T>::addParsedObject(
    const std::string &name,
    std::vector<std::unique_ptr<JsonBaseInfo>> innerFields) {
  auto info = std::make_unique<JsonObjectInfo>(name, std::move(innerFields));
  fieldsInfo.push_back(std::move(info));
}
} // namespace common
