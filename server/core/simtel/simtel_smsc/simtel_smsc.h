#pragma once

#include "server/app/config/smsc_config/smsc_config.h"

namespace server {
class SimtelSmsc {
private:
  const unsigned int smsTtlMs = 0;
  std::string cdrJsonFilePath = "";

public:
  explicit SimtelSmsc(const SmscConfig &config);
};
} // namespace server
