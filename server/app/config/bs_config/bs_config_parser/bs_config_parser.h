#pragma once

#include "common/json/json_parser/json_parser.h"

#include <vector>

#include "server/app/config/bs_config/bs_config/bs_config.h"
#include "server/app/config/mme_config/mme_config.h"

namespace server {
class BsConfigParser : public common::JsonParser<std::vector<BsConfig>> {
private:
  std::vector<MmeConfig> mme;

  std::vector<BsConfig> configs{};
  BsConfig curConfig;

  explicit BsConfigParser(const std::vector<MmeConfig> &mme_);

  virtual void initFields() override;

  virtual std::expected<std::vector<BsConfig>, std::string>
  parseJson(const nlohmann::json &json) override;

public:
  static std::unique_ptr<BsConfigParser>
  create(const std::vector<MmeConfig> &mme);
};
} // namespace server
