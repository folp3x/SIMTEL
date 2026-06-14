#pragma once

#include "common/json/json_parser/json_parser.h"

#include <vector>

#include "server/app/config/bs_config/bs_config/bs_config.h"

namespace server {
class BsConfigParser : public common::JsonParser<std::vector<BsConfig>> {
private:
  std::vector<BsConfig> configs{};
  BsConfig curConfig;

  BsConfigParser() = default;

  std::expected<std::vector<BsConfig>, std::string>
  parseJson(const nlohmann::json &json);

public:
  virtual void initFields() override;

  static std::unique_ptr<BsConfigParser> create();
};
} // namespace server
