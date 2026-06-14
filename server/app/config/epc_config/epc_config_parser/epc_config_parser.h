#pragma once

#include "common/json/json_parser/json_parser.h"

#include <vector>

#include "server/app/config/epc_config/epc_config/epc_config.h"

namespace server {
class EpcConfigParser : public common::JsonParser<EpcConfig> {
private:
  EpcConfig config;

  EpcConfigParser() = default;

  std::expected<EpcConfig, std::string> parseJson(const nlohmann::json &json);

  void initTtlField();
  void initHlrAccessParamsField();
  void initCdrAccessParamsField();

public:
  virtual void initFields() override;

  static std::unique_ptr<EpcConfigParser> create();
};
} // namespace server
