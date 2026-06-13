#pragma once

#include <cstdint>

namespace server {
struct SmscConfig {
  unsigned int ttlMs = 0;
  std::string cdrAccessFilePath = "";
};
} // namespace server
