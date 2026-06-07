#pragma once

#include "common/app/config/config_parser/config_parser.h"

#include "client/app/config/config/config.h"

namespace client {
class ConfigParser : public common::ConfigParser<Config> {
private:
  ConfigParser() = default;

  void initImeiField();
  void initImsiField();
  void initIpField();
  void initAddressBookFilePathField();

  virtual void initFields() override;

public:
  static std::unique_ptr<ConfigParser> create();
};
} // namespace client
