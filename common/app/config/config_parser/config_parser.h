#pragma once

#include "common/json/json_parser/json_parser.h"

#include <concepts>

#include "common/app/config/config/config.h"

namespace common {
// базовый класс для парсинга конфигурации из JSON
template <std::derived_from<Config> T>
class ConfigParser : public JsonParser<T> {
protected:
  T config{};

  ConfigParser() = default;

  void initIpField();
  void initPortField();
  void initLocField();

  virtual void initFields() override;

public:
  static std::unique_ptr<ConfigParser> create();

  virtual std::expected<T, std::string>
  parseJson(const nlohmann::json &json) override;
};
} // namespace common

#include "config_parser_impl.h"
