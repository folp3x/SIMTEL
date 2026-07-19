#pragma once

#include "common/json/info/json_value_info/json_value_info.h"

namespace common {
// абстрактный класс с информацией для парсинга контейнера из JSON
template <typename Container>
class JsonContainerInfo : public JsonValueInfo<Container> {
private:
  using T = std::ranges::range_value_t<Container>;

protected:
  virtual std::optional<std::string>
  parseContainer(const nlohmann::json &json) = 0;

  const std::function<std::string(const T &)> elemValidateFunc;

public:
  JsonContainerInfo(
      Container *value,
      const std::function<std::string(const T &)> &elemValidateFunc_ = nullptr);

  virtual std::optional<std::string> parse(const nlohmann::json &json) override;
};
} // namespace common

#include "json_container_info_impl.h"
