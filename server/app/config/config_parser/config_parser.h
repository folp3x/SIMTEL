#pragma once

#include "common/app/config/config_parser/config_parser.h"
#include "server/app/config/config/config.h"

namespace server {
class ConfigParser : public common::ConfigParser<Config> {
private:
  ConfigParser() = default;

  void initBsFilePathField();
  void initEpcFilePathField();
  void initSmscConfigField();

  virtual void initFields() override;

public:
  static std::unique_ptr<ConfigParser> create();
};
} // namespace server
