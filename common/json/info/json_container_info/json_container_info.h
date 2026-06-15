#pragma once

#include "common/json/info/json_field_info/json_field_info.h"

namespace common {
// абстрактный класс с информацией для парсинга контейнера из JSON
template <typename Container>
class JsonContainerInfo : public JsonFieldInfo<Container> {
protected:
  virtual std::expected<Container, std::string>
  parseContainer(const nlohmann::json &fieldJson) = 0;

public:
  JsonContainerInfo(
      const std::string &name,
      const std::function<void(const Container &)> &successCallback,
      const std::function<std::string(const Container &)> &checkFn = nullptr);

  virtual std::optional<std::string> parse(const nlohmann::json &json,
                                           bool finalParse = true) override;
};
} // namespace common

#include "json_container_info_impl.h"
