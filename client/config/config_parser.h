#pragma once

#include "common/app/config/config_parser.h"

#include "config.h"

namespace client {
class ConfigParser : public common::ConfigParser<Config> {
private:
  ConfigParser() = default;

  void initImeiField();
  void initImsiField();

  virtual void initFields() override;

public:
  static std::unique_ptr<ConfigParser> create();
};
} // namespace client
