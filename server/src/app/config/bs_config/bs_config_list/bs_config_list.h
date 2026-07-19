#pragma once

#include <vector>

#include "app/config/bs_config/bs_config/bs_config.h"
#include "app/config/mme_config/mme_config.h"
#include "common/json/json_parsable/json_parsable.h"

namespace server {
class BsConfigList : public common::JsonParsable {
private:
  std::vector<MmeConfig> mmeConfigs;

  std::vector<BsConfig> configs{};
  BsConfig curConfig;

  bool mmeConfigExists(unsigned int id) const;

  virtual std::unique_ptr<common::BaseJsonInfo> getJsonRootInfo() override;

public:
  explicit BsConfigList(const std::vector<MmeConfig> &mmeConfigs_);

  std::vector<BsConfig> getConfigs() const;
};
} // namespace server
