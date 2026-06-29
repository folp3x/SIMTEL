#pragma once

#include <string>

namespace server {
struct EpcConfig {
  unsigned int ttlSec = 0;
  std::string hlrSqliteFilePath = "";
};
} // namespace server
